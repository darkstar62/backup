// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/server/backup_set_impl.h"

#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "IceUtil/UUID.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/backup_impl.h"
#include "backend/btrfs/server/util.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kBackupTypeIncremental;
using backup_proto::kStatusBackupCreateFailed;
using backup_proto::kStatusBackupSetNotFound;
using backup_proto::kStatusOk;
using boost::filesystem::path;
using std::fstream;
using std::ios;
using std::string;
using std::vector;

namespace backup {

BackupSetImpl::~BackupSetImpl() {
  LOG(INFO) << mName << ": Creating backup set descriptor";
  path desc_path = path(mPath) / "backup_set_descriptor.cfg";

  // Convert the list of backup sets to a data stream we can write.
  Ice::OutputStreamPtr out = Ice::createOutputStream(ic_);
  out->write(descriptor_);
  out->writePendingObjects();

  vector<uint8_t> out_stream;
  out->finished(out_stream);

  fstream output(desc_path.string().c_str(), ios::out | ios::binary);
  output.write(
      reinterpret_cast<const char*>(&out_stream.at(0)), out_stream.size());
  output.close();
}

StatusPtr BackupSetImpl::Init(const Ice::Current& current) {
  if (initialized_) {
    return new StatusImpl(kStatusOk);
  }

  ic_ = current.adapter->getCommunicator();

  // Look for our backup set -- it'll be in the path given to us by the
  // backend.
  path set_path(mPath);
  if (!boost::filesystem::exists(set_path)) {
    LOG(ERROR) << "Backup set " << mName << " failed to init: "
               << "No such directory: " << mPath;
    return new StatusImpl(kStatusBackupSetNotFound,
                          "No such directory: " + mPath);
  }
  if (!boost::filesystem::is_directory(set_path)) {
    LOG(ERROR) << "Backup set " << mName << " failed to init: "
               << "Not a directory: " << mPath;
    return new StatusImpl(kStatusBackupSetNotFound,
                          "Not a directory: " + mPath);
  }

  // Try and find a backup set descriptor.  If there is none, we start
  // with an empty one.
  path desc_path = set_path / "backup_set_descriptor.cfg";
  if (boost::filesystem::exists(desc_path)) {
    LOG(INFO) << mName << ": Reading backup set descriptor";
    fstream input(desc_path.string().c_str(), ios::in | ios::binary);
    vector<uint8_t> in_stream;
    in_stream.resize(boost::filesystem::file_size(desc_path));
    input.read(reinterpret_cast<char*>(&in_stream.at(0)), in_stream.size());
    input.close();

    Ice::InputStreamPtr in = Ice::createInputStream(ic_, in_stream);
    in->read(descriptor_);
    in->readPendingObjects();
  }

  initialized_ = true;
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupSetImpl::CreateBackup(
    const backup_proto::BackupOptions& options,
    backup_proto::BackupPrx& backup_ref,
    const Ice::Current& current) {
  LOG(INFO) << mName << ": Create backup: " << options.description;

  // Consistency check the backup parameters -- incremental backups require
  // a previous backup having been performed.  We take the latest backup as
  // the basis for the incremental.
  string increment_of_id = "_INVALID_";
  if (options.type == kBackupTypeIncremental) {
    // Find the most recent backup and use its ID.
    uint64_t max_time = 0;
    backup_proto::BackupList::iterator max_iter = descriptor_.backups.end();

    for (backup_proto::BackupList::iterator iter = descriptor_.backups.begin();
         iter != descriptor_.backups.end(); ++iter) {
      if ((*iter)->get_create_time() > max_time) {
        max_time = (*iter)->get_create_time();
        max_iter = iter;
      }
    }

    if (max_iter == descriptor_.backups.end()) {
      // No latest backup was found -- probably we don't have any backups
      // at all.
      CHECK_EQ(0, descriptor_.backups.size())
          << mName << ": Corrupt backup descriptor";
      LOG(ERROR) << mName << ": Incremental backup requested, but no existing "
                 << "backups found";
      return new StatusImpl(
          kStatusBackupCreateFailed,
          "Incremental backup requested without existing backup");
    }

    VLOG(3) << mName << ": Latest backup ID: " << (*max_iter)->get_id();
    increment_of_id = (*max_iter)->get_id();
  }

  // Create a new directory in the backup set directory, and initialize the
  // backup set there.  From this point on, the backup set itself manages
  // the contents of the directory, including its own backup set descriptor.
  string new_uuid = IceUtil::generateUUID();
  path backup_path = path(mPath) / new_uuid;
  if (!boost::filesystem::create_directory(backup_path)) {
    LOG(ERROR) << mName << ": Could not create directory for new backup set: "
               << backup_path.native();
    return new StatusImpl(
        kStatusBackupCreateFailed,
        "Could not create directory for new backup");
  }

  // Create all the goo in the backup descriptor
  backup_proto::BackupPtr proto_backup = new BackupImpl;
  proto_backup->set_description(options.description);
  proto_backup->set_type(options.type);
  proto_backup->set_id(new_uuid);
  proto_backup->set_increment_of_id(increment_of_id);
  proto_backup->set_create_time(time(NULL));
  proto_backup->set_size_in_mb(options.size_in_mb);
  proto_backup->set_path(backup_path.string());

  // Initialize the backup, creating the filesystem image, initially blank.
  StatusPtr init_retval = proto_backup->Init();
  if (!init_retval->ok()) {
    LOG(ERROR) << mName << ": Failed to initialize backup: "
               << init_retval->ToString();
    if (rmdir(backup_path.native().c_str()) == -1) {
      LOG(ERROR)
          << mName << ": Additionally, failed to delete backup directory "
          << backup_path.native() << ": " << string(strerror(errno));
    }
    return init_retval;
  }

  descriptor_.backups.push_back(proto_backup);

  // Return the created backup set proto
  backup_ref = GetProxyById<backup_proto::Backup>(
      current.adapter, proto_backup,
      proto_backup->get_id_as_identity());
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupSetImpl::EnumerateBackups(
    backup_proto::BackupPtrList& backup_list_ref,
    const Ice::Current& current) {
  // Return a list of all the backup sets we're managing.
  for (int i = 0; i < descriptor_.backups.size(); ++i) {
    backup_proto::BackupPtr msg = descriptor_.backups.at(i);
    backup_list_ref.push_back(GetProxyById<backup_proto::Backup>(
            current.adapter, msg, msg->get_id_as_identity()));
  }

  return new StatusImpl(kStatusOk);
}

}  // namespace backup
