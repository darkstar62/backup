// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/client/btrfs_backup_set.h"

#include <vector>

#include "backend/btrfs/client/btrfs_backup.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/public/backup.h"
#include "backend/public/backup_set.h"
#include "glog/logging.h"

using backup_proto::StatusPtr;
using std::vector;

namespace backup {

bool BtrfsBackupSet::Init() {
  // Try to initialize the server-facing backup set instance.
  StatusPtr retval = server_set_->Init();
  if (!retval->ok()) {
    LOG(ERROR) << "Error initializing backup set on server: "
               << retval->ToString();
    return false;
  }

  return true;
}

vector<Backup*> BtrfsBackupSet::EnumerateBackups() {
  // The backups are stored in the backup descriptor for the backup set.
  backup_proto::BackupPtrList backup_list;
  StatusPtr retval = server_set_->EnumerateBackups(backup_list);
  if (!retval->ok()) {
    LOG(ERROR) << description() << ": Could not enumerate backups: "
               << retval->ToString();
    // FIXME(#4): Implement error propagation
    return vector<Backup*>();
  }

  vector<Backup*> backups;
  for (backup_proto::BackupPtrList::iterator iter = backup_list.begin();
       iter != backup_list.end(); ++iter) {
    BtrfsBackup* backup = new BtrfsBackup(*iter);
    if (!backup->Init()) {
      LOG(ERROR) << "Failed to initialize backup "
                 << (*iter)->get_description();
      continue;
    }
    backups.push_back(backup);
  }

  return backups;
}

Backup* BtrfsBackupSet::CreateIncrementalBackup(const BackupOptions& options) {
  // Creating an incremental backup requires a previous backup.  Though it's
  // possible to create an incremental backup using any backup increment, it's
  // also kinda pointless to do it from one in the middle.  So to simplify
  // things, we use the last created backup as the base.
  backup_proto::BackupOptions btrfs_options;
  btrfs_options.type = backup_proto::kBackupTypeIncremental;
  btrfs_options.description = options.description();
  btrfs_options.size_in_mb = options.size_in_mb();

  backup_proto::BackupPrx backup_proto;
  StatusPtr retval = server_set_->CreateBackup(btrfs_options, backup_proto);
  if (!retval->ok()) {
    LOG(ERROR) << description() << ": Could not create backup: "
               << retval->ToString();
    return NULL;
  }

  BtrfsBackup* backup = new BtrfsBackup(backup_proto);

  LOG(INFO) << "Success";
  return backup;
}

Backup* BtrfsBackupSet::CreateFullBackup(const BackupOptions& options) {
  backup_proto::BackupOptions btrfs_options;
  btrfs_options.type = backup_proto::kBackupTypeFull;
  btrfs_options.description = options.description();
  btrfs_options.size_in_mb = options.size_in_mb();

  backup_proto::BackupPrx backup_proto;
  StatusPtr retval = server_set_->CreateBackup(btrfs_options, backup_proto);
  if (!retval->ok()) {
    LOG(ERROR) << description() << ": Could not create backup: "
               << retval->ToString();
    return NULL;
  }

  BtrfsBackup* backup = new BtrfsBackup(backup_proto);

  LOG(INFO) << "Success";
  return backup;
}

}  // namespace backup
