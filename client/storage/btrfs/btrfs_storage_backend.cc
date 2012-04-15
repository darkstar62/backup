// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "client/storage/btrfs/btrfs_backup_set.h"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backend::BtrfsBackendServicePrx;
using backend::BackupSetList;
using std::ostringstream;
using std::string;
using std::vector;

DEFINE_int32(num_rpc_threads, 1, "Number of threads for RPC handling");

namespace client {
class BackupSet;

BtrfsStorageBackend::~BtrfsStorageBackend() {
}

bool BtrfsStorageBackend::Init() {
  ostringstream service_str;
  service_str << "BtrfsBackendService:default -h " << host_ << " -p " << port_;
  Ice::ObjectPrx base = ic_->stringToProxy(service_str.str());
  service_ = BtrfsBackendServicePrx::checkedCast(base);

  service_->Ping();

  LOG(INFO) << "BTRFS backend successfully initialized on " << host_ << ":"
            << port_;
  return true;
}

vector<BackupSet*> BtrfsStorageBackend::EnumerateBackupSets() {
  vector<BackupSet*> backup_sets;

  // Send an RPC to the backend to get the backup sets.
  BackupSetList sets = service_->EnumerateBackupSets();

  for (BackupSetList::iterator iter = sets.begin();
       iter != sets.end(); ++iter) {
    VLOG(3) << "Backup set: " << iter->name;
    BtrfsBackupSet* backup_set = new BtrfsBackupSet(iter->name);
    backup_sets.push_back(backup_set);
  }
  return backup_sets;
}

BackupSet* BtrfsStorageBackend::CreateBackupSet(string name) {
  return NULL;
}

}  // namespace client
