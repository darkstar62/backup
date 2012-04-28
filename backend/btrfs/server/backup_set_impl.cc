// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <fstream>
#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "IceUtil/UUID.h"
#include "backend/btrfs/server/backup_set_impl.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupCreateFailed;
using backup_proto::kStatusBackupSetNotFound;
using backup_proto::kStatusOk;
using boost::filesystem::path;
using std::fstream;
using std::ios;
using std::string;
using std::vector;

namespace backup {

Ice::CommunicatorPtr BackupSetFactory::ic_ = NULL;

BackupSetServerImpl::~BackupSetServerImpl() {
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

StatusPtr BackupSetServerImpl::Init(const Ice::Current&) {
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

  return new StatusImpl(kStatusOk);
}

StatusPtr BackupSetServerImpl::CreateBackup(
    backup_proto::BackupType type,
    const backup_proto::BackupOptions& options,
    backup_proto::BackupPtr& backup_ref,
    const Ice::Current&) {
  LOG(INFO) << mName << ": Create backup: " << options.description;

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
  backup_proto::BackupPtr proto_backup = new backup_proto::Backup;
  proto_backup->description = options.description;
  proto_backup->type = type;
  proto_backup->id.name = new_uuid;
  proto_backup->increment_of_id.name = "foo";
  descriptor_.backups.push_back(proto_backup);

  // Return the created backup set proto
  //backup_ref = GetProxyById<backup_proto::BackupSet>(proto_set, proto_set->id);
  backup_ref = proto_backup;
  return new StatusImpl(kStatusOk);
}

}  // namespace backup
