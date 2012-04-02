// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <string>
#include <vector>

#include "client/storage/btrfs/btrfs_storage_backend.h"

using std::string;
using std::vector;

namespace client {
class BackupSet;

bool BtrfsStorageBackend::Init() {
  // TODO: Initialize the RPC stub and ensure we can talk to it.
  return true;
}

vector<BackupSet*> BtrfsStorageBackend::EnumerateBackupSets() {
  vector<BackupSet*> backup_sets;
  return backup_sets;
}

BackupSet* BtrfsStorageBackend::CreateBackupSet(string name) {
  return NULL;
}

}  // namespace client
