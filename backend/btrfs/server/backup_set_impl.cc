// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "Ice/Ice.h"
#include "backend/btrfs/server/backup_set_impl.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupSetNotFound;
using backup_proto::kStatusOk;

namespace backup {

StatusPtr BackupSetServerImpl::Init(const Ice::Current& /* current */) {
  // Look for our backup set -- it'll be in the path given to us by the
  // backend.
  boost::filesystem::path set_path(mPath);
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
  return new StatusImpl(kStatusOk);
}

}  // namespace backup
