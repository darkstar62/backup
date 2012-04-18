// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/btrfs/btrfs_backup_set.h"
#include "backend/btrfs/btrfs_storage_backend.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backup_proto::BackupSetList;
using backup_proto::BackupSetMessage;
using backup_proto::BtrfsBackendServicePrx;
using std::ostringstream;
using std::string;
using std::vector;

DEFINE_int32(num_rpc_threads, 1, "Number of threads for RPC handling");

namespace backup {
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
    BtrfsBackupSet* backup_set = new BtrfsBackupSet(*iter);
    backup_sets.push_back(backup_set);
  }
  return backup_sets;
}

bool BtrfsStorageBackend::CreateBackupSet(const string name,
                                          BackupSet** backup_set) {
  BackupSetMessage msg;
  bool retval = service_->CreateBackupSet(name, msg);
  if (!retval) {
    return retval;
  }

  BtrfsBackupSet* set = new BtrfsBackupSet(msg);
  *backup_set = set;
  return true;
}

}  // namespace backup
