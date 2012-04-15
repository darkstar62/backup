// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_H_

#include <stdint.h>

#include "Ice/Ice.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "base/macros.h"
#include "boost/scoped_ptr.hpp"

namespace backend {

class BtrfsBackend {
 public:
  BtrfsBackend(Ice::CommunicatorPtr ic, const uint32_t port)
      : ic_(ic), port_(port) {}
  ~BtrfsBackend() {}

  bool Init();
  void Start();

 private:
  // The hosted RPC service.
  Ice::ObjectAdapterPtr adapter_;
  Ice::CommunicatorPtr ic_;

  // Port the server should listen on.
  const uint32_t port_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsBackend);
};

}  // namespace backend

#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_H_
