// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_STORAGE_BACKEND_H_
#define BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_STORAGE_BACKEND_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "backend/btrfs/server/btrfs_backend_service.proto.h"
#include "backend/public/storage_backend.h"
#include "base/macros.h"

namespace backup {
class BackupSet;

class BtrfsStorageBackend : public StorageBackend {
 public:
  BtrfsStorageBackend(Ice::CommunicatorPtr ic, const std::string host,
                      const std::string port)
      : StorageBackend(),
        host_(host),
        port_(port),
        ic_(ic) {
  }

  virtual ~BtrfsStorageBackend();

  // Initialize the storage backend.  This should be called after setting
  // configuration parameters.
  virtual bool Init();

  // Enumerate all the backup sets contained in the storage system.  The
  // return value is a vector of pointers to BackupSet objects (ownership
  // of which remains with the backend).
  virtual std::vector<BackupSet*> EnumerateBackupSets();

  // Attempt to create a backup set on the storage backend representing
  // the passed backup parameters.  If successful, this returns a new BackupSet
  // representing the set (ownership remains with the StorageBackend), pointed to
  // by the passed pointer and the function returns true.  Otherwise, the pointer
  // is left alone and the function returns false.
  //
  // For BTRFS, this simply adds an entry for the backup set in the backup
  // descriptor -- it does not modify or create any filesystems.
  virtual bool CreateBackupSet(const std::string name, BackupSet** backup_set);

  // Retrieve the backup set with the given name.  If the backup set cannot
  // be found or an error occurs, this returns NULL.
  virtual BackupSet* GetBackupSet(std::string name);

 private:
  // Host to connect to.
  std::string host_;

  // Port to connect to.
  std::string port_;

  // The RPC controller.
  backup_proto::BtrfsBackendServicePrx service_;
  Ice::CommunicatorPtr ic_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsStorageBackend);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_CLIENT_BTRFS_STORAGE_BACKEND_H_
