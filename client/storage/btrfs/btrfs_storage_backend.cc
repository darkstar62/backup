// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.pb.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/thread/future.hpp"
#include "boost/thread/thread.hpp"
#include "casock/rpc/protobuf/client/RPCCallController.h"
#include "casock/rpc/protobuf/client/RPCCallHandlerFactoryImpl.h"
#include "casock/rpc/protobuf/client/RPCResponseHandler.h"
#include "casock/rpc/sigio/protobuf/client/RPCClientProxy.h"
#include "casock/sigio/base/Dispatcher.h"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "glog/logging.h"
#include "google/protobuf/stubs/once.h"

using backend::BtrfsBackendService;
using backend::EmptyMessage;
using boost::promise;
using boost::posix_time::seconds;
using boost::unique_future;
using casock::rpc::protobuf::client::RPCCallController;
using casock::rpc::protobuf::client::RPCCallHandlerFactoryImpl;
using casock::rpc::protobuf::client::RPCResponseHandler;
using casock::rpc::sigio::protobuf::client::RPCClientProxy;
using casock::sigio::base::Dispatcher;
using google::protobuf::GoogleOnceInit;
using google::protobuf::NewCallback;
using std::string;
using std::vector;

namespace client {
class BackupSet;

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(rpc_init);

void RunDispatcher() {
  Dispatcher::getInstance()->waitForever();
}

void InitRPC() {
  Dispatcher::initialize();
  boost::thread thread(RunDispatcher);
}

}  // namespace

BtrfsStorageBackend::~BtrfsStorageBackend() {
}

void Handler(boost::promise<bool>* task) {
  LOG(INFO) << "Handler";
  task->set_value(true);
}

bool BtrfsStorageBackend::Init() {
  // Initialize the RPC system, but only ever do it once.
  GoogleOnceInit(&rpc_init, &InitRPC);

  // Create our RPC controllers.
  call_handler_factory_.reset(new RPCCallHandlerFactoryImpl);
  rpc_proxy_.reset(
      new RPCClientProxy(*Dispatcher::getInstance(),
                         host_, port_, *call_handler_factory_.get()));
  rpc_controller_.reset(rpc_proxy_->buildRPCCallController());
  rpc_proxy_->setNumCallHandlers(1);
  service_.reset(new BtrfsBackendService::Stub(rpc_proxy_.get()));

  // Send a ping to ensure we can communicate.
  EmptyMessage request, response;
  promise<bool> task;
  unique_future<bool> retval = task.get_future();
  service_->Ping(rpc_controller_.get(), &request, &response, NewCallback(&Handler, &task));
  retval.timed_wait(seconds(5));
  if (!retval.is_ready() || !retval.get()) {
    LOG(WARNING) << "Error waiting for ping result";
    return false;
  }

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
