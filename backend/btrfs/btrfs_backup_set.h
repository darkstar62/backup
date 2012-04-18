// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKUP_SET_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKUP_SET_H_

#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/public/backup_set.h"

namespace backup {

class Backup;

// A BTRFS-based remote backup set.  These backup sets represent sets of data
// on a remote server.
class BtrfsBackupSet : public BackupSet {
 public:
  BtrfsBackupSet(const backup_proto::BackupSetMessage& set_msg)
      : BackupSet(set_msg.name) {}
  virtual ~BtrfsBackupSet() {}

  // Enumerate all backup increments stored in this backup set.  The
  // returned vector contains const pointers to all backup instances; ownership
  // of the backup instances remains with the BackupSet.
  virtual std::vector<Backup*> EnumerateBackups() {
    return std::vector<Backup*>();
  }

  // Attempt to create an incremental backup on the storage backend representing
  // the passed backup parameters.  This doesn't actually initiate a backup;
  // this only allocates resources on the storage backend for the backup and
  // prepares it to run.  The backup is considered incomplete until a backup
  // operation completes successfully.  If successful, this function returns
  // a new Backup instance (of which ownership remains with the BackupSet).
  // Otherwise, NULL is returned and the storage backend is left unchanged.
  //
  // The incremental backup uses the last backup instance as a baseline.
  virtual Backup* CreateIncrementalBackup(std::string description) { return NULL; }

  // Similar to CreateIncrementalBackup(), but this creates a full backup.
  virtual Backup* CreateFullBackup(std::string description) { return NULL; }
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKUP_SET_H_

