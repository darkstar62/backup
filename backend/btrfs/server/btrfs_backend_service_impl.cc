// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/server/btrfs_backend_service_impl.h"

#include <fstream>
#include <vector>

#include "Ice/Ice.h"
#include "IceUtil/UUID.h"
#include "backend/btrfs/proto/btrfs_backend_service.proto.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/backup_set_impl.h"
#include "backend/btrfs/server/global.h"
#include "backend/btrfs/server/util.h"
#include "base/string.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupSetCreateFailed;
using backup_proto::kStatusBackupSetNotFound;
using backup_proto::kStatusOk;
using boost::filesystem::path;
using std::fstream;
using std::ios;
using std::string;
using std::vector;

namespace backup {

BtrfsBackendServiceImpl::BtrfsBackendServiceImpl(const std::string& path)
    : BtrfsBackendService(),
      path_(boost::filesystem::path(path)),
      backup_descriptor_(new backup_proto::BackupDescriptor) {
  // Verify the path given exists and is a directory.
  if (!boost::filesystem::exists(path_) ||
      !boost::filesystem::is_directory(path_)) {
    LOG(FATAL) << "Specified path doesn't exist or isn't a directory.";
  }
}

BtrfsBackendServiceImpl::~BtrfsBackendServiceImpl() {
  LOG(INFO) << "Creating backup descriptor";
  path desc_path = path_ / "backup_descriptor.cfg";

  // Convert the list of backup sets to a data stream we can write.
  Ice::OutputStreamPtr out = Ice::createOutputStream(IceObjects::Instance()->ic);
  out->write(backup_descriptor_);
  out->writePendingObjects();

  vector<uint8_t> out_stream;
  out->finished(out_stream);

  fstream output(desc_path.string().c_str(), ios::out | ios::binary);
  output.write(reinterpret_cast<const char*>(&out_stream.at(0)), out_stream.size());
  output.close();
}

bool BtrfsBackendServiceImpl::Init() {
  // Check if we have a backup directory descriptor file.
  path desc_path = path_ / "backup_descriptor.cfg";
  if (boost::filesystem::exists(desc_path)) {
    LOG(INFO) << "Reading backup descriptor";
    fstream input(desc_path.string().c_str(), ios::in | ios::binary);
    vector<uint8_t> in_stream;
    in_stream.resize(boost::filesystem::file_size(desc_path));
    input.read(reinterpret_cast<char*>(&in_stream.at(0)), in_stream.size());
    input.close();

    Ice::InputStreamPtr in = Ice::createInputStream(
        IceObjects::Instance()->ic, in_stream);
    in->read(backup_descriptor_);
    in->readPendingObjects();
  }

  return true;
}

void BtrfsBackendServiceImpl::Ping(const Ice::Current& current) {
  VLOG(3) << "Ping() requested";
  // Nothing to do here, we just return.
}

backup_proto::BackupSetPtrList BtrfsBackendServiceImpl::EnumerateBackupSets(
    const Ice::Current& current) {
  VLOG(3) << "EnumerateBackupSets() requested";
  // Return a list of all the backup sets we're managing.
  backup_proto::BackupSetPtrList retval;
  for (int i = 0; i < backup_descriptor_->backup_sets.size(); ++i) {
    backup_proto::BackupSetPtr msg = backup_descriptor_->backup_sets.at(i);
    retval.push_back(GetProxyById<backup_proto::BackupSet>(msg, msg->id));
  }
  return retval;
}

StatusPtr BtrfsBackendServiceImpl::CreateBackupSet(
    const string& name, backup_proto::BackupSetPrx& set_ref,
    const Ice::Current& current) {
  // Create a new directory in the backup set directory, and initialize the
  // backup set there.  From this point on, the backup set itself manages
  // the contents of the directory, including its own backup set descriptor.
  string new_uuid = IceUtil::generateUUID();
  path backup_set_path = path_ / new_uuid;
  if (!boost::filesystem::create_directory(backup_set_path)) {
    LOG(ERROR) << "Could not create directory for new backup set: "
               << backup_set_path.native();
    return new StatusImpl(
        kStatusBackupSetCreateFailed,
        "Could not create directory for new backup set");
  }

  // Create all the goo in the backup descriptor
  BackupSetImpl* proto_set = new BackupSetImpl;
  backup_proto::BackupSetPtr proto_ptr(proto_set);
  proto_set->mName = name;
  proto_set->id.name = new_uuid;
  proto_set->mPath = backup_set_path.string();
  backup_descriptor_->backup_sets.push_back(proto_set);

  // Return the created backup set proto
  set_ref = GetProxyById<backup_proto::BackupSet>(proto_ptr, proto_set->id);
  return new StatusImpl(kStatusOk);
}

StatusPtr BtrfsBackendServiceImpl::GetBackupSet(
    const string& id, backup_proto::BackupSetPrx& set_ref,
    const Ice::Current& current) {
  for (backup_proto::BackupSetList::iterator iter =
           backup_descriptor_->backup_sets.begin();
       iter != backup_descriptor_->backup_sets.end();
       ++iter) {
    if ((*iter)->id.name == id) {
      set_ref = GetProxyById<backup_proto::BackupSet>(*iter, (*iter)->id);
      return new StatusImpl(kStatusOk);
    }
  }
  return new StatusImpl(kStatusBackupSetNotFound,
                        "Backup set specified could not be found");
}

}  // namespace backup
