// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_PROTO_STATUS_IMPL_H_
#define BACKUP_BACKEND_BTRFS_PROTO_STATUS_IMPL_H_

#include <iostream>
#include <sstream>
#include <string>

#include "Ice/Ice.h"
#include "backend/btrfs/proto/status.proto.h"
#include "base/macros.h"
#include "glog/logging.h"

namespace backup_proto {

// Implementation of the ICE Status class.
class StatusImpl : public Status {
 public:
  // Various constructors -- the second allows for a default reason so one is
  // not necessary.
  StatusImpl() : Status() {}
  StatusImpl(StatusEnum type, std::string reason="")
      : Status(type, reason) {}

  // Return whether the status object represents an OK response or not.  If the
  // status is OK, this returns true; otherwise false is returned (i.e. anything
  // not kStatusOK is false).
  virtual bool ok(const Ice::Current& current) { return id == kStatusOk; }

  // Convert the status object to a string representation.  Useful for logging.
  virtual std::string ToString(const Ice::Current& current);

 private:
  DISALLOW_COPY_AND_ASSIGN(StatusImpl);
};

class StatusFactory : public Ice::ObjectFactory {
 public:
  virtual Ice::ObjectPtr create(const std::string& type) {
    CHECK_EQ(Status::ice_staticId(), type);
    return new StatusImpl;
  }
  virtual void destroy() {}

  static void Init(Ice::CommunicatorPtr ic) {
    ic->addObjectFactory(new StatusFactory, Status::ice_staticId());
  }
};

}  // namespace backup_proto

#endif  // BACKUP_BACKEND_BTRFS_PROTO_STATUS_IMPL_H_
