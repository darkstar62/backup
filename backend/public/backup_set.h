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

  // Set/get the ID of the backup.  This is only used when returning
  // a Backup* object to the client -- setting this when creating
  // the backup backend-side does nothing.
  BackupOptions& set_id(const std::string& id) {
    id_ = id;
    return *this;
  }
  const std::string id() const { return id_; }

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
  std::string id_;
  std::string description_;
  uint64_t size_in_mb_;

  DISALLOW_COPY_AND_ASSIGN(BackupOptions);
};

// A representation of a backup set.  Backup sets contain one or more
// backup increments (which can be incremental relative to another backup
// increment or a full backup).  Subclasses of this are expected to implement
// specific storage backend functionality.
class BackupSet {
 public:
  BackupSet(const std::string& id,
            const std::string& description)
      : id_(id),
        description_(description) {}
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

  // Return the ID of this backup set.
  const std::string id() const { return id_; }

  // Return the description of this backup set.
  const std::string description() const { return description_; }

 private:
  // ID of the backup set.  Backup sets can have the same name, but IDs
  // must be unique.
  const std::string id_;

  // Description of this backup set.
  const std::string description_;

  DISALLOW_COPY_AND_ASSIGN(BackupSet);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_PUBLIC_BACKUP_SET_H_
