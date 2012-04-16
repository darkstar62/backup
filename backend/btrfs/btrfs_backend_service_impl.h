// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_

#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/btrfs/file_formats.pb.h"
#include "base/macros.h"

namespace backend {

// BTRFS backend service implementation.  This implements the RPC protocol between
// the client and the server, and is responsible for most of what goes on server-
// side for this backend.
class BtrfsBackendServiceImpl : public BtrfsBackendService {
 public:
  // Initialize the backend service.  The backup sets are enumberated from the given
  // path on the server filesystem.
  BtrfsBackendServiceImpl(const std::string& path);
  virtual ~BtrfsBackendServiceImpl();

  // Initialize the backend.  Returns true if successful, false otherwise.
  bool Init();

  // Initiate a ping to verify the service is alive.
  virtual void Ping(const Ice::Current&);

  // Get all the backup sets managed by the backend.
  virtual std::vector<BackupSetMessage> EnumerateBackupSets(
      const Ice::Current&);

 private:
  // Path to the backup sets on the server filesystem.  This path is used for all
  // operations the server needs to perform.
  const std::string path_;

  BackupDescriptor backup_descriptor_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsBackendServiceImpl);
};

}  // namespace backend
#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
