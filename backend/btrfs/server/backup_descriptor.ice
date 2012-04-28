// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_BACKUP_DESCRIPTOR_ICE_
#define BACKUP_BACKEND_BTRFS_BACKUP_DESCRIPTOR_ICE_

#include <Ice/Identity.ice>
#include <backend/btrfs/proto/status.ice>

module backup_proto {

enum BackupType {
  kBackupTypeFull,
  kBackupTypeIncremental
};

class Backup {
  // Unique identity of the backup set.
  Ice::Identity id;

  // Description of the backup
  string description;

  // Backup type.
  BackupType type;

  // Size in MB of the backup, as reported by the client.
  long size_in_mb;

  // If this backup is an incremental backup, this is the ID of the backup
  // this one is an incremental backup of.
  Ice::Identity increment_of_id;
};

sequence<Backup> BackupList;

// The BackupSetDescriptor describes all of the backups contained within a
// backup set.
struct BackupSetDescriptor {
  // List of all backups in the backup set.
  BackupList backups;
};

struct BackupOptions {
  string description;
  long size_in_mb;
};

class BackupSet {
  // These functions are designed to allow the client and server halves
  // of the BackupSet object interact more freely, rather than having to
  // pass around the service and make direct calls on it.

  // Initialize the backup set descriptor and get the backup set ready
  // to use for the client.
  Status Init();

  // Create a backup.
  Status CreateBackup(BackupType type, BackupOptions options,
                      out Backup backup_ref);

  // Various accessors for the client-side.
  string get_name();
  void set_name(string name);

  // Unique identifier of the backup set in the backend.  This also forms
  // the identity used to proxy BackupSet objects between the client and
  // the server.
  ["private"] Ice::Identity id;

  // Descriptive name of the backup set.
  ["private"] string mName;

  // Full path to the backup set.
  ["private"] string mPath;
};

sequence<BackupSet*> BackupSetPtrList;
sequence<BackupSet> BackupSetList;

// The BackupDescriptor is the overall description of a directory containing
// one or more backup sets.
struct BackupDescriptor {
  // List of all backup sets managed by the backend.
  BackupSetList backup_sets;

  // The next ID to assign to a new backup set.
  long next_id;
};

};  // module backup_proto

#endif  // BACKUP_BACKEND_BTRFS_BACKUP_DESCRIPTOR_ICE_
