// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PROTO_BACKUP_ICE_
#define BACKUP_BACKEND_PROTO_BACKUP_ICE_

#include <Ice/Identity.ice>
#include <backend/btrfs/proto/status.ice>

module backup_proto {

enum BackupType {
  kBackupTypeFull,
  kBackupTypeIncremental
};

class Backup {
  // Initialize the backup on the server.  This creates the BTRFS filesystem
  // image if it doesn't exist and populates it with symlinks if incremental.
  Status Init();

  // Accessors for the client
  string get_id();
  long get_create_time();
  string get_description();
  BackupType get_type();
  long get_size_in_mb();
  string get_increment_of_id();

  // Unique identity of the backup set.
  Ice::Identity id;

  // Creation time of the backup
  long create_time;

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

sequence<Backup*> BackupPtrList;
sequence<Backup> BackupList;

};  // module backup_proto

#endif  // BACKUP_BACKEND_PROTO_BACKUP_ICE_
