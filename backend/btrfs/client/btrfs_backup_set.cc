// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <string>
#include <vector>

#include "backend/btrfs/client/btrfs_backup_set.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/public/backup.h"
#include "backend/public/backup_set.h"
#include "glog/logging.h"

using backup_proto::StatusPtr;
using backup_proto::kBackupTypeFull;
using backup_proto::kBackupTypeIncremental;
using std::string;
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

std::vector<Backup*> BtrfsBackupSet::EnumerateBackups() {
  // The backups are stored in the backup descriptor for the backup set.
  return vector<Backup*>();
}

Backup* BtrfsBackupSet::CreateIncrementalBackup(const BackupOptions& options) {
  // Creating an incremental backup requires a previous backup.  Though it's
  // possible to create an incremental backup using any backup increment, it's
  // also kinda pointless to do it from one in the middle.  So to simplify
  // things, we use the last created backup as the base.
  backup_proto::BackupOptions btrfs_options;
  btrfs_options.description = options.description();
  btrfs_options.size_in_mb = options.size_in_mb();

  backup_proto::BackupPtr backup;
  StatusPtr retval = server_set_->CreateBackup(
      kBackupTypeIncremental, btrfs_options, backup);
  if (!retval->ok()) {
    LOG(ERROR) << description() << ": Could not create backup: "
               << retval->ToString();
    return NULL;
  }

  LOG(INFO) << "Success";
  return NULL;
}

Backup* BtrfsBackupSet::CreateFullBackup(const BackupOptions& options) {
  backup_proto::BackupOptions btrfs_options;
  btrfs_options.description = options.description();
  btrfs_options.size_in_mb = options.size_in_mb();

  backup_proto::BackupPtr backup;
  StatusPtr retval = server_set_->CreateBackup(
      kBackupTypeFull, btrfs_options, backup);
  if (!retval->ok()) {
    LOG(ERROR) << description() << ": Could not create backup: "
               << retval->ToString();
    return NULL;
  }

  LOG(INFO) << "Success";
  return NULL;
}

}  // namespace backup
