// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_CLIENT_STORAGE_PUBLIC_BACKUP_SET_H_
#define BACKUP_CLIENT_STORAGE_PUBLIC_BACKUP_SET_H_

#include <string>
#include <vector>

namespace client {

class Backup;

// A representation of a backup set.  Backup sets contain one or more
// backup increments (which can be incremental relative to another backup
// increment or a full backup).  Subclasses of this are expected to implement
// specific storage backend functionality.
class BackupSet {
 public:
  BackupSet() {}
  virtual ~BackupSet() {}

  // Enumerate all backup increments stored in this backup set.  The
  // returned vector contains const pointers to all backup instances; ownership
  // of the backup instances remains with the BackupSet.
  virtual vector<Backup*> EnumerateBackups() = 0;

  // Attempt to create an incremental backup on the storage backend representing
  // the passed backup parameters.  This doesn't actually initiate a backup;
  // this only allocates resources on the storage backend for the backup and
  // prepares it to run.  The backup is considered incomplete until a backup
  // operation completes successfully.  If successful, this function returns
  // a new Backup instance (of which ownership remains with the BackupSet).
  // Otherwise, NULL is returned and the storage backend is left unchanged.
  //
  // The incremental backup uses the last backup instance as a baseline.
  virtual Backup* CreateIncrementalBackup(string description) = 0;

  // Similar to CreateIncrementalBackup(), but this creates a full backup.
  virtual Backup* CreateFullBackup(string description) = 0;
};

}  // namespace client
#endif  // BACKUP_CLIENT_STORAGE_PUBLIC_BACKUP_SET_H_
