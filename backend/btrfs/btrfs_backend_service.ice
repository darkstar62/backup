// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_ICE_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_ICE_

#include <backend/btrfs/backup_descriptor.ice>
#include <backend/btrfs/status.ice>

module backup_proto {

interface BtrfsBackendService {
  // Initiate a ping to verify the service is alive.
  void Ping();

  // Get all the backup sets managed by the backend.
  BackupSetPtrList EnumerateBackupSets();

  // Create a new backup set with the given name.  If successful, the passed
  // BackupSetMessage is filled with the details of the set and the function
  // returns true.  Otherwise, the passed message is left alone and the function
  // returns false.
  Status CreateBackupSet(string name, out BackupSet* set);

  // Retrieve the backup set with the given name.  If successful, the passed
  // BackupSetMessage is filled withthe details of the set and the function
  // returns true.  Otherwise, the passed message is left alone and the function
  // returns false.
  Status GetBackupSet(string name, out BackupSet* set);
};

};  // module backend

#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_ICE_
