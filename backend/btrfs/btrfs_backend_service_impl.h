// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
#define BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_

#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "backend/btrfs/backup_descriptor.proto.h"
#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/btrfs/status.proto.h"
#include "backend/btrfs/status_impl.h"
#include "base/macros.h"
#include "boost/filesystem.hpp"

namespace backup {

// BTRFS backend service implementation.  This implements the RPC protocol between
// the client and the server, and is responsible for most of what goes on server-
// side for this backend.
class BtrfsBackendServiceImpl : public backup_proto::BtrfsBackendService {
 public:
  // Initialize the backend service.  The backup sets are enumberated from the given
  // path on the server filesystem.
  BtrfsBackendServiceImpl(
      Ice::CommunicatorPtr ic, Ice::ObjectAdapterPtr adapter,
      const std::string& path);
  virtual ~BtrfsBackendServiceImpl();

  // Initialize the backend.  Returns true if successful, false otherwise.
  bool Init();

  // Initiate a ping to verify the service is alive.
  virtual void Ping(const Ice::Current& current);

  // Get all the backup sets managed by the backend.
  virtual backup_proto::BackupSetPtrList EnumerateBackupSets(
      const Ice::Current& current);

  // Create a new backup set.
  virtual backup_proto::StatusPtr CreateBackupSet(
      const std::string& name,
      backup_proto::BackupSetPrx& set_ref,
      const Ice::Current& current);

  // Retrieve an existing backup set.
  virtual backup_proto::StatusPtr GetBackupSet(
      const std::string& name,
      backup_proto::BackupSetPrx& set_ref,
      const Ice::Current& current);

 private:
  // Utility function to map from an ICE object / ID to its proxy.
  template<typename T>
  typename T::ProxyType GetProxyById(
      typename T::PointerType& obj, Ice::Identity& id);

  // Path to the backup sets on the server filesystem.  This path is used for all
  // operations the server needs to perform.
  const boost::filesystem::path path_;

  backup_proto::BackupDescriptor backup_descriptor_;

  Ice::CommunicatorPtr ic_;
  Ice::ObjectAdapterPtr adapter_;

  DISALLOW_COPY_AND_ASSIGN(BtrfsBackendServiceImpl);
};

template<typename T>
typename T::ProxyType BtrfsBackendServiceImpl::GetProxyById(
    typename T::PointerType& obj, Ice::Identity& id) {
  Ice::ObjectPtr found_obj = adapter_->find(id);
  typename T::ProxyType proxy;
  if (!found_obj.get()) {
    // Object isn't mapped into ICE; do that now.
    LOG(INFO) << "Added object " << id.name;
    proxy = T::ProxyType::uncheckedCast(adapter_->add(obj, id));
  } else {
    // We have the object, just grab the proxy.
    CHECK_EQ(obj.get(), found_obj.get());
    LOG(INFO) << "Got existing object " << id.name;
    proxy = T::ProxyType::uncheckedCast(adapter_->createProxy(id));
  }
  return proxy;
}

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_BTRFS_BACKEND_SERVICE_IMPL_H_
