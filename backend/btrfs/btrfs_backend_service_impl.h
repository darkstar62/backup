// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_

#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "base/macros.h"

namespace backup {

// BTRFS backend service implementation.  This implements the RPC protocol between
// the client and the server, and is responsible for most of what goes on server-
// side for this backend.
class BtrfsBackendServiceImpl : public backup_proto::BtrfsBackendService {
 public:
  // Initialize the backend service.  The backup sets are enumberated from the given
  // path on the server filesystem.
  BtrfsBackendServiceImpl(Ice::CommunicatorPtr ic, const std::string& path);
  virtual ~BtrfsBackendServiceImpl();

  // Initialize the backend.  Returns true if successful, false otherwise.
  bool Init();

  // Initiate a ping to verify the service is alive.
  virtual void Ping(const Ice::Current& current);

  // Get all the backup sets managed by the backend.
  virtual backup_proto::BackupSetList EnumerateBackupSets(
      const Ice::Current& current);

  // Create a new backup set.
  virtual bool CreateBackupSet(const std::string& name,
                               backup_proto::BackupSetMessage& set_ref,
                               const Ice::Current& current);

 private:
  // Path to the backup sets on the server filesystem.  This path is used for all
  // operations the server needs to perform.
  const std::string path_;

  backup_proto::BackupDescriptor backup_descriptor_;

  Ice::CommunicatorPtr ic_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsBackendServiceImpl);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
