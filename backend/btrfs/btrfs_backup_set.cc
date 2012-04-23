// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/btrfs_backup_set.h"
#include "backend/public/backup.h"
#include "backend/public/backup_set.h"

namespace backup {

Backup* BtrfsBackupSet::CreateIncrementalBackup(const BackupOptions& options) {
  return NULL;
}

Backup* BtrfsBackupSet::CreateFullBackup(const BackupOptions& options) {
  return NULL;
}

}  // namespace backup
