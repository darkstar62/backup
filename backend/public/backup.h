// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PUBLIC_BACKUP_H_
#define BACKUP_BACKEND_PUBLIC_BACKUP_H_

#include <time.h>
#include <string>

#include "base/macros.h"
#include "backend/public/file_list.h"

namespace backup {

enum BackupType {
  kBackupTypeInvalid,
  kBackupTypeIncremental,
  kBackupTypeFull
};

class Backup{
 public:
  Backup(const std::string& id, const std::string& description)
      : id_(id),
        description_(description),
        type_(kBackupTypeInvalid) {}
  virtual ~Backup() {}

  // Actually perform a backup, given a list of files to backup.  Returns true
  // if the backup is successful; false otherwise.
  // TODO: Make this asynchronous.
  virtual bool DoBackup(const FileList& filelist) = 0;

  // Open the specified backup instance represented by this instance object
  // and return back a string path representing the instance's mounted
  // location, visible from the client computer.  Implementations of this
  // function need to be OS-aware and return the appropriate style path
  // for the operating system.
  virtual const std::string OpenAndGetRestorePath() = 0;

  // Accessors for the various data elements.
  const std::string id() const { return id_; }

  const std::string description() const { return description_; }
  void set_description(const std::string& description) {
    description_ = description;
  }

  const time_t create_time() const { return create_time_; }
  void set_create_time(time_t create_time) { create_time_ = create_time; }

  const BackupType type() const { return type_; }
  void set_type(BackupType type) { type_ = type; }

  const std::string increment_of_id() const { return increment_of_id_; }
  void set_increment_of_id(std::string id) { increment_of_id_ = id; }

 private:
  // Unique ID of this backup.  This is used to coordinate between the client
  // and the backend.
  const std::string id_;

  // Descriptive string for the backup.  This need not be unique and isn't
  // used as a key for anything.
  std::string description_;

  // Creation time of the backup in UNIX seconds.
  time_t create_time_;

  // Type of the backup -- incremental or full.
  BackupType type_;

  // ID of the backup this backup is an increment of (if incremental).  This
  // isn't used for anything if the backup type is full.
  std::string increment_of_id_;

  DISALLOW_COPY_AND_ASSIGN(Backup);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_PUBLIC_BACKUP_H_
