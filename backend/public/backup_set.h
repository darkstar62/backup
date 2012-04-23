// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PUBLIC_BACKUP_SET_H_
#define BACKUP_BACKEND_PUBLIC_BACKUP_SET_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace backup {

class Backup;

// A set of options to pass in when creating a backup increment.
class BackupOptions {
 public:
  BackupOptions()
      : description_(""), size_in_mb_(0) {}
  ~BackupOptions() {}

  // Set/get the friendly description of the backup.
  BackupOptions& set_description(const std::string& description) {
    description_ = description;
    return *this;
  }
  const std::string description() const { return description_; }

  // Set/get the size of the backup in megabytes.
  BackupOptions& set_size_in_mb(uint64_t size_in_mb) {
    size_in_mb_ = size_in_mb;
    return *this;
  }
  const uint64_t size_in_mb() const { return size_in_mb_; }

 private:
  std::string description_;
  uint64_t size_in_mb_;
};

// A representation of a backup set.  Backup sets contain one or more
// backup increments (which can be incremental relative to another backup
// increment or a full backup).  Subclasses of this are expected to implement
// specific storage backend functionality.
class BackupSet {
 public:
  BackupSet(const std::string& description) : description_(description) {}
  virtual ~BackupSet() {}

  // Enumerate all backup increments stored in this backup set.  The
  // returned vector contains const pointers to all backup instances; ownership
  // of the backup instances remains with the BackupSet.
  virtual std::vector<Backup*> EnumerateBackups() = 0;

  // Attempt to create an incremental backup on the storage backend representing
  // the passed backup parameters.  This doesn't actually initiate a backup;
  // this only allocates resources on the storage backend for the backup and
  // prepares it to run.  The backup is considered incomplete until a backup
  // operation completes successfully.  If successful, this function returns
  // a new Backup instance (of which ownership remains with the BackupSet).
  // Otherwise, NULL is returned and the storage backend is left unchanged.
  //
  // The incremental backup uses the last backup instance as a baseline.
  virtual Backup* CreateIncrementalBackup(const BackupOptions& options) = 0;

  // Similar to CreateIncrementalBackup(), but this creates a full backup.
  virtual Backup* CreateFullBackup(const BackupOptions& options) = 0;

  // Return the description of this backup set.
  const std::string description() const { return description_; }

 private:
  // Description of this backup set.
  const std::string description_;
};

}  // namespace backup
#endif  // BACKUP_BACKEND_PUBLIC_BACKUP_SET_H_
