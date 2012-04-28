// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "Ice/Ice.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/server/backup_impl.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusOk;

namespace backup {

StatusPtr BackupImpl::Init(const Ice::Current&) {
  LOG(INFO) << "BackupImpl::Init";
  return new StatusImpl(kStatusOk);
}

}  // namespace backup
