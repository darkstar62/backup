// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PUBLIC_STORAGE_BACKEND_H_
#define BACKUP_BACKEND_PUBLIC_STORAGE_BACKEND_H_

#include <string>
#include <vector>

#include "base/macros.h"

namespace backup {

class BackupSet;

// Base class StorageBackend.  Subclasses implement the specific storage-backend
// functionality.
class StorageBackend {
 public:
  StorageBackend() {}
  virtual ~StorageBackend() {}

  // Initialize the storage backend.  This should be called after setting
  // configuration parameters.
  virtual bool Init() = 0;

  // Enumerate all the backup sets contained in the storage system.  The
  // return value is a vector of pointers to BackupSet objects (ownership
  // of which remains with the backend).
  virtual std::vector<BackupSet*> EnumerateBackupSets() = 0;

  // Attempt to create a backup set on the storage backend representing
  // the passed backup parameters.  If successful, this returns a new BackupSet
  // representing the set (ownership remains with the StorageBackend), pointed
  // to by the passed pointer and the function returns true.  Otherwise, the
  // pointer is left alone and the function returns false.
  virtual bool CreateBackupSet(std::string name, BackupSet** backup_set) = 0;

  // Retrieve the backup set with the given name.  If the backup set cannot
  // be found or an error occurs, this returns NULL.
  virtual BackupSet* GetBackupSet(std::string name) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(StorageBackend);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_PUBLIC_STORAGE_BACKEND_H_
