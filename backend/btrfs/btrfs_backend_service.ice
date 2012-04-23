// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_ICE_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_ICE_

#include <backend/btrfs/status.ice>

module backup_proto {

struct BackupSetMessage {
  // Unique identifier of the backup set in the backend.  Only positive
  // integers are valid.
  long id;

  // Descriptive name of the backup set.
  string name;
};

sequence<BackupSetMessage> BackupSetList;

struct BackupDescriptor {
  // List of all backup sets managed by the backend.
  BackupSetList backup_sets;

  // The next ID to assign to a new backup set.
  long next_id;
};

interface BtrfsBackendService {
  // Initiate a ping to verify the service is alive.
  void Ping();

  // Get all the backup sets managed by the backend.
  BackupSetList EnumerateBackupSets();

  // Create a new backup set with the given name.  If successful, the passed
  // BackupSetMessage is filled with the details of the set and the function
  // returns true.  Otherwise, the passed message is left alone and the function
  // returns false.
  Status CreateBackupSet(string name, out BackupSetMessage set);

  // Retrieve the backup set with the given name.  If successful, the passed
  // BackupSetMessage is filled withthe details of the set and the function
  // returns true.  Otherwise, the passed message is left alone and the function
  // returns false.
  Status GetBackupSet(string name, out BackupSetMessage set);
};

};  // module backend

#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_ICE_
