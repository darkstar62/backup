// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_CLIENT_STORAGE_PUBLIC_STORAGE_BACKEND_H_
#define BACKUP_CLIENT_STORAGE_PUBLIC_STORAGE_BACKEND_H_

#include <string>
#include <vector>

namespace client {

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
  // representing the set (ownership remains with the StorageBackend).
  // Otherwise this function returns NULL.
  virtual BackupSet* CreateBackupSet(std::string name) = 0;
};

}  // namespace client
#endif  // BACKUP_CLIENT_STORAGE_PUBLIC_STORAGE_BACKEND_H_
