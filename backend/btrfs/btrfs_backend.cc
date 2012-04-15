// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/btrfs_backend.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "boost/scoped_ptr.hpp"
#include "casock/rpc/protobuf/server/RPCCallHandlerFactoryImpl.h"
#include "casock/rpc/sigio/protobuf/server/RPCServerProxy.h"
#include "casock/sigio/base/Dispatcher.h"
#include "casock/util/Logger.h"
#include "glog/logging.h"

using casock::rpc::protobuf::server::RPCCallHandlerFactoryImpl;
using casock::rpc::sigio::protobuf::server::RPCServerProxy;
using casock::sigio::base::Dispatcher;

namespace backend {

bool BtrfsBackend::Init() {
  service_.reset(new BtrfsBackendServiceImpl);
  return true;
}

void BtrfsBackend::Start() {
  // LOGGER->setDebugLevel (LOW_LEVEL);
  Dispatcher::initialize();
  RPCCallHandlerFactoryImpl callHandlerFactory(service_.get());
  RPCServerProxy proxy(callHandlerFactory, *Dispatcher::getInstance(), port_);
  proxy.setNumCallHandlers(1);
  proxy.start();
  Dispatcher::getInstance()->waitForever();
  Dispatcher::destroy();
}

}  // namespace backend
