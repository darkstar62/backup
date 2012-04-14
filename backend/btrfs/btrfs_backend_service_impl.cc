// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/btrfs_backend_service.pb.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "glog/logging.h"

using ::google::protobuf::RpcController;
using ::google::protobuf::Closure;

namespace backend {

void BtrfsBackendServiceImpl::Ping(RpcController* controller,
                                   const EmptyMessage* request,
                                   EmptyMessage* response,
                                   Closure* cb_final) {
  LOG(INFO) << "Ping!";
  cb_final->Run();
}

}  // namespace backend
