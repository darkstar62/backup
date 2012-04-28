// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_BACKUP_DESCRIPTOR_ICE_
#define BACKUP_BACKEND_BTRFS_BACKUP_DESCRIPTOR_ICE_

#include <Ice/Identity.ice>
#include <backend/btrfs/proto/status.ice>
#include <backend/btrfs/server/backup.ice>
#include <backend/btrfs/server/backup_set.ice>

module backup_proto {

// The BackupSetDescriptor describes all of the backups contained within a
// backup set.
struct BackupSetDescriptor {
  // List of all backups in the backup set.
  BackupList backups;
};

// The BackupDescriptor is the overall description of a directory containing
// one or more backup sets.
class BackupDescriptor {
  // List of all backup sets managed by the backend.
  BackupSetList backup_sets;
};

};  // module backup_proto

#endif  // BACKUP_BACKEND_BTRFS_BACKUP_DESCRIPTOR_ICE_
