// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BASE_ICE_H_
#define BACKUP_BASE_ICE_H_

#include "Ice/Ice.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/backup_set_impl.h"

// Initialize ICE and register any ICE object factories.
static Ice::CommunicatorPtr InitializeIce(int argc, char* argv[]) {
  Ice::CommunicatorPtr ic = Ice::initialize(argc, argv);

  // Register ICE object factories.
  backup_proto::StatusFactory::Init(ic);
  backup::BackupSetFactory::Init(ic);

  return ic;
}

#endif  // BACKUP_BASE_ICE_H_
