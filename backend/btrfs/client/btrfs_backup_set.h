// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_BACKUP_SET_H_
#define BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_BACKUP_SET_H_

#include <stdint.h>
#include <vector>

#include "Ice/Ice.h"
#include "base/macros.h"
#include "backend/btrfs/proto/backup_descriptor.proto.h"
#include "backend/public/backup_set.h"

namespace backup {

class Backup;

// A BTRFS-based remote backup set.  These backup sets represent sets of data
// on a remote server.
class BtrfsBackupSet : public BackupSet {
 public:
  BtrfsBackupSet(backup_proto::BackupSetPrx set_msg)
      : BackupSet(set_msg->get_id(), set_msg->get_name()),
        server_set_(set_msg) {}
  virtual ~BtrfsBackupSet() {}

  // Initialize the backup set.  This does housekeeping things like creating
  // the backup set descriptor if it doesn't exist, and verifying integrity
  // of the descriptor against the backup itself (quickly).
  bool Init();

  // Enumerate all backup increments stored in this backup set.  The
  // returned vector contains const pointers to all backup instances; ownership
  // of the backup instances remains with the BackupSet.
  virtual std::vector<Backup*> EnumerateBackups();

  // Attempt to create an incremental backup on the storage backend representing
  // the passed backup parameters.  This doesn't actually initiate a backup;
  // this only allocates resources on the storage backend for the backup and
  // prepares it to run.  The backup is considered incomplete until a backup
  // operation completes successfully.  If successful, this function returns
  // a new Backup instance (of which ownership remains with the BackupSet).
  // Otherwise, NULL is returned and the storage backend is left unchanged.
  //
  // The incremental backup uses the last backup instance as a baseline.
  virtual Backup* CreateIncrementalBackup(const BackupOptions& options);

  // Similar to CreateIncrementalBackup(), but this creates a full backup.
  //
  // For BTRFS, this creates a new filesystem image of the approprate (sparse)
  // size, able to contain the full uncompressed contents of the backup.
  // Obviously compression will bring the requirements down, but we don't
  // rely on that always working.
  virtual Backup* CreateFullBackup(const BackupOptions& options);

 private:
  // Server-side backup set representation.  This contains methods that we'll
  // need to be able to communicate client-side actions to the server.
  backup_proto::BackupSetPrx server_set_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsBackupSet);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_BACKUP_SET_H_

