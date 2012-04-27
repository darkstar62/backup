// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <string>
#include <vector>

#include "backend/btrfs/client/btrfs_backup_set.h"
#include "backend/btrfs/client/btrfs_storage_backend.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/backup_descriptor.proto.h"
#include "backend/btrfs/server/btrfs_backend_service.proto.h"
#include "base/string.h"
#include "boost/filesystem.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backup_proto::BtrfsBackendServicePrx;
using backup_proto::StatusPtr;
using std::string;
using std::vector;

DEFINE_int32(num_rpc_threads, 1, "Number of threads for RPC handling");

namespace backup {
class BackupSet;

BtrfsStorageBackend::~BtrfsStorageBackend() {
}

bool BtrfsStorageBackend::Init() {
  Ice::ObjectPrx base = ic_->stringToProxy(
      "BtrfsBackendService:default -h " + ToString(host_) + " -p " +
      ToString(port_));
  service_ = BtrfsBackendServicePrx::checkedCast(base);

  service_->Ping();

  LOG(INFO) << "BTRFS backend successfully initialized on " << host_ << ":"
            << port_;
  return true;
}

vector<BackupSet*> BtrfsStorageBackend::EnumerateBackupSets() {
  vector<BackupSet*> backup_sets;

  // Send an RPC to the backend to get the backup sets.
  backup_proto::BackupSetPtrList sets = service_->EnumerateBackupSets();

  for (backup_proto::BackupSetPtrList::iterator iter = sets.begin();
       iter != sets.end(); ++iter) {
    VLOG(3) << "Backup set: " << (*iter)->get_name();
    BtrfsBackupSet* backup_set = new BtrfsBackupSet(*iter);
    if (!backup_set->Init()) {
      LOG(WARNING) << "Backup set " << (*iter)->get_name()
                   << " failed to initialize";
      continue;
    }
    backup_sets.push_back(backup_set);
  }
  return backup_sets;
}

bool BtrfsStorageBackend::CreateBackupSet(const string name,
                                          BackupSet** backup_set) {
  backup_proto::BackupSetPrx msg;
  StatusPtr retval = service_->CreateBackupSet(name, msg);
  if (!retval->ok()) {
    LOG(ERROR) << "Error creating backup: " << retval->ToString();
    return false;
  }

  BtrfsBackupSet* set = new BtrfsBackupSet(msg);
  *backup_set = set;
  return true;
}

BackupSet* BtrfsStorageBackend::GetBackupSet(const string name) {
  backup_proto::BackupSetPrx msg;
  StatusPtr retval = service_->GetBackupSet(name, msg);
  if (!retval->ok()) {
    LOG(ERROR) << "Error getting backup set: " << retval->ToString();
    return NULL;
  }

  BtrfsBackupSet* set = new BtrfsBackupSet(msg);
  if (!set->Init()) {
    delete set;
    LOG(ERROR) << "Error initializing backup set";
    return NULL;
  }

  return set;
}

}  // namespace backup