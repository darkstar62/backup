// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "glog/logging.h"

using std::vector;

namespace backend {

void BtrfsBackendServiceImpl::Ping(const Ice::Current& current) {
}

vector<BackupSetMessage> BtrfsBackendServiceImpl::EnumerateBackupSets(
    const Ice::Current& current) {
  LOG(INFO) << "Enumerate Backup Sets";
  // Return a list of all the backup sets we're managing.
  // TODO(darkstar62): Enumerate the backup sets and actually return something
  // meaningful.
  vector<BackupSetMessage> retval;
  for (int i = 0; i < 10000; ++i) {
    BackupSetMessage msg;
    msg.name = "Foo";
    retval.push_back(msg);
  }
  return retval;
}

}  // namespace backend
