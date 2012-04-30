// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/client/btrfs_backup.h"

#include "backend/btrfs/proto/backup.proto.h"
#include "backend/btrfs/proto/status.proto.h"
#include "glog/logging.h"

using backup_proto::StatusPtr;

namespace backup {

BtrfsBackup::BtrfsBackup(backup_proto::BackupPrx backup_service)
    : Backup(backup_service->get_id(), backup_service->get_description()),
      backup_service_(backup_service) {
  // Populate all the rest of the Backup base-class options from the server.
  set_create_time(backup_service->get_create_time());
  set_type(backup_service->get_type() == backup_proto::kBackupTypeIncremental ?
           kBackupTypeIncremental : kBackupTypeFull);
  set_increment_of_id(backup_service->get_increment_of_id());
}

bool BtrfsBackup::Init() {
  // Initialize the server-side backup.  All file manipulation has to happen
  // there, as the client half of this probably isn't running on the same
  // machine.
  StatusPtr retval = backup_service_->Init();
  if (!retval->ok()) {
    LOG(ERROR) << "Backup init failed server-side: " << retval->ToString();
    return false;
  }

  return true;
}

}  // namespace backup
