// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

// Note that everything in this file defines GLOBAL variables for use by
// the BTRFS backend *server* only.

#ifndef BACKUP_BACKEND_BTRFS_SERVER_GLOBAL_H_
#define BACKUP_BACKEND_BTRFS_SERVER_GLOBAL_H_

#include "Ice/Ice.h"
#include "base/macros.h"

namespace backup {

class IceObjects {
 public:
  static IceObjects* Instance() {
    if (!backup::IceObjects::instance_) {
      instance_ = new IceObjects;
    }
    return instance_;
  }

  Ice::CommunicatorPtr ic;
  Ice::ObjectAdapterPtr adapter;
  static IceObjects* instance_;

 private:
  IceObjects() {}
  ~IceObjects() {}

  DISALLOW_COPY_AND_ASSIGN(IceObjects);
};

}  // namespace backup

#endif  // BACKUP_BACKEND_BTRFS_SERVER_GLOBAL_H_
