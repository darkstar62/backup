// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.pb.h"
#include "casock/rpc/protobuf/client/RPCCallController.h"
#include "casock/rpc/protobuf/client/RPCCallHandlerFactoryImpl.h"
#include "casock/rpc/protobuf/client/RPCResponseHandler.h"
#include "casock/rpc/sigio/protobuf/client/RPCClientProxy.h"
#include "casock/sigio/base/Dispatcher.h"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "glog/logging.h"

using backend::BtrfsBackendService;
using backend::EmptyMessage;
using casock::rpc::protobuf::client::RPCCallController;
using casock::rpc::protobuf::client::RPCCallHandlerFactoryImpl;
using casock::rpc::protobuf::client::RPCResponseHandler;
using casock::rpc::sigio::protobuf::client::RPCClientProxy;
using casock::sigio::base::Dispatcher;
using google::protobuf::NewCallback;
using std::string;
using std::vector;

namespace client {
class BackupSet;

BtrfsStorageBackend::~BtrfsStorageBackend() {
  Dispatcher::destroy();
}

void Handler() {
  LOG(INFO) << "Handler";
  Dispatcher::getInstance()->deactivate();
}

bool BtrfsStorageBackend::Init() {
  // Initialize the RPC system.
  LOG(INFO) << "Init()";
  Dispatcher::initialize();
  call_handler_factory_.reset(new RPCCallHandlerFactoryImpl);
  rpc_proxy_.reset(
      new RPCClientProxy(*Dispatcher::getInstance(),
                         host_, port_, *call_handler_factory_.get()));
  rpc_controller_.reset(rpc_proxy_->buildRPCCallController());
  rpc_proxy_->setNumCallHandlers(1);

  // Create the service stub.
  service_.reset(new BtrfsBackendService::Stub(rpc_proxy_.get()));

  // Send a ping to ensure we can communicate.
  ping_ok_ = false;
  EmptyMessage request, response;
  service_->Ping(rpc_controller_.get(), &request, &response, NewCallback(&Handler));
  Dispatcher::getInstance()->waitForever();
  LOG(INFO) << "Done";

  return true;
}

vector<BackupSet*> BtrfsStorageBackend::EnumerateBackupSets() {
  vector<BackupSet*> backup_sets;
  return backup_sets;
}

BackupSet* BtrfsStorageBackend::CreateBackupSet(string name) {
  return NULL;
}

}  // namespace client
