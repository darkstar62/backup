// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_CLIENT_STORAGE_BTRFS_BTRFS_STORAGE_BACKEND_H_
#define BACKUP_CLIENT_STORAGE_BTRFS_BTRFS_STORAGE_BACKEND_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.pb.h"
#include "base/macros.h"
#include "boost/scoped_ptr.hpp"
#include "boost/thread/future.hpp"
#include "casock/rpc/asio/protobuf/client/RPCClientProxy.h"
#include "casock/rpc/asio/protobuf/client/RPCSocketClientFactoryImpl.h"
#include "casock/rpc/protobuf/client/RPCCallController.h"
#include "casock/rpc/protobuf/client/RPCCallHandlerFactoryImpl.h"
#include "client/storage/public/storage_backend.h"

namespace client {
class BackupSet;

class BtrfsStorageBackend : public StorageBackend {
 public:
  BtrfsStorageBackend(const std::string host, const std::string port)
      : StorageBackend(),
        host_(host),
        port_(port) {
  }

  virtual ~BtrfsStorageBackend();

  // Initialize the storage backend.  This should be called after setting
  // configuration parameters.
  virtual bool Init();

  // Enumerate all the backup sets contained in the storage system.  The
  // return value is a vector of pointers to BackupSet objects (ownership
  // of which remains with the backend).
  virtual std::vector<BackupSet*> EnumerateBackupSets();

  // Attempt to create a backup set on the storage backend representing
  // the passed backup parameters.  If successful, this returns a new BackupSet
  // representing the set (ownership remains with the StorageBackend).
  // Otherwise this function returns NULL.
  virtual BackupSet* CreateBackupSet(std::string name);

 private:
  // A do-nothing handler that simply returns true status to the passed in
  // task.  Useful for making asynchronous calls synchronous.
  void OnRpcDone(casock::rpc::protobuf::client::RPCCallController* rpc,
                 boost::promise<bool>* task);

  // Host to connect to.
  std::string host_;

  // Port to connect to.
  std::string port_;

  // The RPC controller.
  boost::scoped_ptr<casock::rpc::asio::protobuf::client::RPCSocketClientFactoryImpl>
      socket_factory_;
  boost::scoped_ptr<casock::rpc::protobuf::client::RPCCallHandlerFactoryImpl>
      call_handler_factory_;
  boost::scoped_ptr<casock::rpc::asio::protobuf::client::RPCClientProxy>
      rpc_proxy_;


  // The backend proxy service stub.
  boost::scoped_ptr<backend::BtrfsBackendService> service_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsStorageBackend);
};

}  // namespace client
#endif  // BACKUP_CLIENT_STORAGE_BTRFS_BTRFS_STORAGE_BACKEND_H_
