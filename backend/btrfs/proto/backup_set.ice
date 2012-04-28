// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_PROTO_BACKUP_SET_ICE_
#define BACKUP_BACKEND_BTRFS_PROTO_BACKUP_SET_ICE_

#include <Ice/Identity.ice>
#include <backend/btrfs/proto/backup.ice>
#include <backend/btrfs/proto/status.ice>

module backup_proto {

// Options to use in creating the backups.
struct BackupOptions {
  // Type of the backup (full or incremental).
  BackupType type;

  // Description of the backup.  This is only used for descriptive
  // purposes for the user, and is not keyed on at all.
  string description;

  // Estimated size of the backup.  The BTRFS filesystem is created this
  // big (but sparse), and compression is used during backup.  This just
  // ensures we have enough space for the backup.
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
  Status CreateBackup(BackupOptions options, out Backup* backup_ref);

  // Enumerate the backups in the set.
  Status EnumerateBackups(out BackupPtrList backup_list_ref);

  // Various accessors for the client-side.
  string get_id();

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

};  // module backup_proto
#endif  // BACKUP_BACKEND_BTRFS_PROTO_BACKUP_SET_ICE_
