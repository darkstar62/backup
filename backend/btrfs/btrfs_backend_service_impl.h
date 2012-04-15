// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_

#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "base/macros.h"

namespace backend {

class BtrfsBackendServiceImpl : public BtrfsBackendService {
 public:
  BtrfsBackendServiceImpl() {}
  virtual ~BtrfsBackendServiceImpl() {}

  virtual void Ping(const Ice::Current&);

  virtual std::vector<BackupSetMessage> EnumerateBackupSets(
      const Ice::Current&);

 private:
  DISALLOW_COPY_AND_ASSIGN(BtrfsBackendServiceImpl);
};

}  // namespace backend
#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
