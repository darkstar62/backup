// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <stdint.h>
#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.pb.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/thread/future.hpp"
#include "boost/thread/thread.hpp"
#include "casock/proactor/asio/base/AsyncProcessor.h"
#include "casock/rpc/asio/protobuf/client/RPCClientProxy.h"
#include "casock/rpc/asio/protobuf/client/RPCSocketClientFactoryImpl.h"
#include "casock/rpc/protobuf/client/RPCCallController.h"
#include "casock/rpc/protobuf/client/RPCCallHandlerFactoryImpl.h"
#include "casock/util/Logger.h"
#include "client/storage/btrfs/btrfs_backup_set.h"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "google/protobuf/stubs/once.h"

using backend::BtrfsBackendService;
using backend::EmptyMessage;
using backend::EnumerateBackupSetsResponse;
using boost::promise;
using boost::posix_time::seconds;
using boost::unique_future;
using casock::proactor::asio::base::AsyncProcessor;
using casock::rpc::protobuf::client::RPCCallController;
using casock::rpc::protobuf::client::RPCCallHandlerFactoryImpl;
using casock::rpc::asio::protobuf::client::RPCClientProxy;
using casock::rpc::asio::protobuf::client::RPCSocketClientFactoryImpl;
using google::protobuf::GoogleOnceInit;
using google::protobuf::NewCallback;
using std::string;
using std::vector;

DEFINE_int32(num_rpc_threads, 1, "Number of threads for RPC handling");

namespace client {
class BackupSet;

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(rpc_init);
boost::scoped_ptr<boost::thread> thread;

void RunDispatcher() {
  LOGGER->setDebugLevel(SILENT);
  AsyncProcessor::getInstance()->run();
}

void InitRPC() {
  AsyncProcessor::initialize();
}

}  // namespace

BtrfsStorageBackend::~BtrfsStorageBackend() {
}

bool BtrfsStorageBackend::Init() {
  // Initialize the RPC system, but only ever do it once.
  GoogleOnceInit(&rpc_init, &InitRPC);

  socket_factory_.reset(new RPCSocketClientFactoryImpl(
      *AsyncProcessor::getInstance(), host_, port_));
  call_handler_factory_.reset(new RPCCallHandlerFactoryImpl);

  rpc_proxy_.reset(
      new RPCClientProxy(*socket_factory_.get(), *call_handler_factory_.get()));
  rpc_proxy_->setNumCallHandlers(FLAGS_num_rpc_threads);
  service_.reset(new BtrfsBackendService::Stub(rpc_proxy_.get()));
  thread.reset(new boost::thread(RunDispatcher));

  sleep(3);

  // Create our RPC controllers.
  // Send a ping to ensure we can communicate.
  VLOG(3) << "Ping";
  EmptyMessage request, response;
  promise<bool> task;
  unique_future<bool> retval = task.get_future();
  RPCCallController* rpc = rpc_proxy_->buildRPCCallController();
  service_->Ping(rpc, &request, &response,
                 NewCallback(this, &BtrfsStorageBackend::OnRpcDone, rpc, &task));
  retval.timed_wait(seconds(1));
  if (!retval.is_ready() || !retval.get()) {
    delete rpc;
    LOG(FATAL) << "Error waiting for ping result";
  }

  LOG(INFO) << "BTRFS backend successfully initialized on " << host_ << ":"
            << port_;
  return true;
}

vector<BackupSet*> BtrfsStorageBackend::EnumerateBackupSets() {
  vector<BackupSet*> backup_sets;

  // Send an RPC to the backend to get the backup sets.
  EmptyMessage request;
  EnumerateBackupSetsResponse response;
  promise<bool> task;
  unique_future<bool> retval = task.get_future();
  RPCCallController* rpc = rpc_proxy_->buildRPCCallController();
  service_->EnumerateBackupSets(
      rpc, &request, &response,
      NewCallback(this, &BtrfsStorageBackend::OnRpcDone, rpc, &task));
  CHECK(!rpc->Failed());
  retval.timed_wait(seconds(1));
  if (!retval.is_ready() || !retval.get()) {
    LOG(WARNING) << "Error retreiving backup sets";
    delete rpc;
    return backup_sets;
  }

  for (int i = 0; i < response.backup_sets_size(); ++i) {
    VLOG(3) << "Backup set: " << response.backup_sets(i).name();
    BtrfsBackupSet* backup_set = new BtrfsBackupSet(
        response.backup_sets(i).name());
    backup_sets.push_back(backup_set);
  }

  return backup_sets;
}

BackupSet* BtrfsStorageBackend::CreateBackupSet(string name) {
  return NULL;
}

void BtrfsStorageBackend::OnRpcDone(RPCCallController* rpc, boost::promise<bool>* task) {
  task->set_value(true);
  delete rpc;
}


}  // namespace client
