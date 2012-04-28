// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_BACKUP_H_
#define BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_BACKUP_H_

#include <string>

#include "backend/public/backup.h"
#include "backend/public/file_list.h"
#include "backend/btrfs/server/backup_descriptor.proto.h"

namespace backup {

// BtrfsBackup is the BTRFS implementation of the Backup interface.
class BtrfsBackup : public Backup {
 public:
  BtrfsBackup(backup_proto::BackupPtr backup_proto)
      : Backup(backup_proto->id.name, backup_proto->description) {
    set_create_time(backup_proto->create_time);
    set_type(backup_proto->type == backup_proto::kBackupTypeIncremental ?
             kBackupTypeIncremental : kBackupTypeFull);
    set_increment_of_id(backup_proto->increment_of_id.name);
  }
  virtual ~BtrfsBackup() {}

  // Actually perform a backup, given a list of files to backup.  Returns true
  // if the backup is successful; false otherwise.
  virtual bool DoBackup(const FileList& filelist) { return false; }

  // Open the specified backup instance represented by this instance object
  // and return back a string path representing the instance's mounted
  // location, visible from the client computer.  Implementations of this
  // function need to be OS-aware and return the appropriate style path
  // for the operating system.
  virtual const std::string OpenAndGetRestorePath() { return ""; }
};

}  // namespace backup

#endif  // BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_BACKUP_H_
