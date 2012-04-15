// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#include <iostream>
#include <sstream>

#include "Ice/Ice.h"
#include "backend/btrfs/btrfs_backend.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "boost/scoped_ptr.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(num_rpc_threads, 1, "Number of RPC threads to use");

using std::ostringstream;

namespace backend {

bool BtrfsBackend::Init() {
  ostringstream host_port_stream;
  host_port_stream << "default -p " << port_;
  adapter_ =
      ic_->createObjectAdapterWithEndpoints("BtrfsBackendServiceAdapter",
                                            host_port_stream.str());
  Ice::ObjectPtr object = new BtrfsBackendServiceImpl;
  adapter_->add(object, ic_->stringToIdentity("BtrfsBackendService"));
  return true;
}

void BtrfsBackend::Start() {
  LOG(INFO) << "BTRFS server listening on port " << port_;
  adapter_->activate();
  ic_->waitForShutdown();
}

}  // namespace backend
