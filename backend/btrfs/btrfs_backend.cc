// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/btrfs_backend.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "boost/scoped_ptr.hpp"
#include "casock/proactor/asio/base/AsyncProcessor.h"
#include "casock/rpc/asio/protobuf/server/RPCServerProxy.h"
#include "casock/rpc/asio/protobuf/server/RPCSocketServerFactoryImpl.h"
#include "casock/rpc/protobuf/server/RPCCallHandlerFactoryImpl.h"
#include "casock/util/Logger.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(num_rpc_threads, 1, "Number of RPC threads to use");

using casock::proactor::asio::base::AsyncProcessor;
using casock::rpc::asio::protobuf::server::RPCServerProxy;
using casock::rpc::asio::protobuf::server::RPCSocketServerFactoryImpl;
using casock::rpc::protobuf::server::RPCCallHandlerFactoryImpl;

namespace backend {

bool BtrfsBackend::Init() {
  service_.reset(new BtrfsBackendServiceImpl);
  return true;
}

void BtrfsBackend::Start() {
  // Set RPC logging level
  LOGGER->setDebugLevel(SILENT);

  AsyncProcessor::initialize();
  RPCSocketServerFactoryImpl server_socket_factory(*AsyncProcessor::getInstance(), port_);
  RPCCallHandlerFactoryImpl callHandlerFactory(service_.get());

  RPCServerProxy proxy(server_socket_factory, callHandlerFactory);
  proxy.setNumCallHandlers(FLAGS_num_rpc_threads);
  proxy.start();

  LOG(INFO) << "BTRFS backend listening on port " << port_;

  // Donate the thread -- from here on out, the RPC service controls the
  // process.
  AsyncProcessor::getInstance()->run();

  // Clean up, we're done.
  AsyncProcessor::destroy();
}

}  // namespace backend
