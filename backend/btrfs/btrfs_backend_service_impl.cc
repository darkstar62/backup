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
  cb_final->Run();
}

void BtrfsBackendServiceImpl::EnumerateBackupSets(
    RpcController* controller,
    const EmptyMessage* request,
    EnumerateBackupSetsResponse* response,
    Closure* cb_final) {
  LOG(INFO) << "Enumerate Backup Sets";
  // Return a list of all the backup sets we're managing.
  // TODO(darkstar62): Enumerate the backup sets and actually return something
  // meaningful.
  for (int i = 0; i < 10000; ++i) {
    response->add_backup_sets()->set_name("Foo");
  }
  cb_final->Run();
}

}  // namespace backend
