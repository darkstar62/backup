// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PROTO_BACKUP_ICE_
#define BACKUP_BACKEND_PROTO_BACKUP_ICE_

#include <Ice/Identity.ice>
#include <backend/btrfs/proto/status.ice>

module backup_proto {

// Different types of backups.  This controls how the server creates and
// manages backups.
enum BackupType {
  kBackupTypeFull,
  kBackupTypeIncremental
};

// An actual backup instance.
class Backup {
  // Initialize the backup on the server.  This creates the BTRFS filesystem
  // image if it doesn't exist and populates it with symlinks if incremental.
  Status Init();

  // Accessors for the client
  string get_id();
  Ice::Identity get_id_as_identity();  // Needed for object lookup.
  void set_id(string id);

  long get_create_time();
  void set_create_time(long time);

  string get_description();
  void set_description(string desc);

  BackupType get_type();
  void set_type(BackupType type);

  long get_size_in_mb();
  void set_size_in_mb(long size);

  string get_increment_of_id();
  void set_increment_of_id(string id);

  string get_path();
  void set_path(string path);

  // Unique identity of the backup.  This identifies the backup within the set,
  // and is used by the client to communicate to the server specifically which
  // backup is in question during operations.
  ["private"] Ice::Identity mId;

  // Creation time of the backup in UNIX seconds since the epoch.
  ["private"] long mCreate_time;

  // Description of the backup.  This is a non-unique descriptive string used
  // soley for user ease.
  ["private"] string mDescription;

  // Backup type -- incremental or full.
  ["private"] BackupType mType;

  // Size in MB of the backup, as reported by the client.  This also forms the
  // size of the BTRFS filesystem created server-side.
  ["private"] long mSize_in_mb;

  // If this backup is an incremental backup, this is the ID of the backup
  // this one is an incremental backup of.  Otherwise, this value is ignored.
  ["private"] Ice::Identity mIncrement_of_id;

  // Full path to the backup directory.
  ["private"] string mPath;
};

// Various sequences.
sequence<Backup*> BackupPtrList;
sequence<Backup> BackupList;

};  // module backup_proto

#endif  // BACKUP_BACKEND_PROTO_BACKUP_ICE_
