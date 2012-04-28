// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_SERVER_UTIL_H_
#define BACKUP_BACKEND_BTRFS_SERVER_UTIL_H_

#include "Ice/Ice.h"
#include "backend/btrfs/server/global.h"
#include "glog/logging.h"

namespace backup {

// Utility function to map from an ICE object / ID to its proxy.
template<typename T>
typename T::ProxyType GetProxyById(
    typename T::PointerType& obj, Ice::Identity& id) {
  Ice::ObjectPtr found_obj = IceObjects::Instance()->adapter->find(id);
  typename T::ProxyType proxy;
  if (!found_obj.get()) {
    // Object isn't mapped into ICE; do that now.
    LOG(INFO) << "Added object " << id.name;
    proxy = T::ProxyType::uncheckedCast(
        IceObjects::Instance()->adapter->add(obj, id));
  } else {
    // We have the object, just grab the proxy.
    CHECK_EQ(obj.get(), found_obj.get());
    LOG(INFO) << "Got existing object " << id.name;
    proxy = T::ProxyType::uncheckedCast(
        IceObjects::Instance()->adapter->createProxy(id));
  }
  return proxy;
}

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_SERVER_UTIL_H_