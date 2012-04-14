// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_

#include "backend/btrfs/btrfs_backend_service.pb.h"
#include "base/macros.h"

namespace backend {

class BtrfsBackendServiceImpl : public BtrfsBackendService {
 public:
  BtrfsBackendServiceImpl() {}
  virtual ~BtrfsBackendServiceImpl() {}

  virtual void Ping(::google::protobuf::RpcController* controller,
                    const EmptyMessage* request,
                    EmptyMessage* response,
                    ::google::protobuf::Closure* cb_final);

 private:
  DISALLOW_COPY_AND_ASSIGN(BtrfsBackendServiceImpl);
};

}  // namespace backend
#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
