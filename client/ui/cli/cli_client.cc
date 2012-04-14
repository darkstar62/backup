// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <signal.h>

#include "casock/util/Logger.h"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "glog/logging.h"

using client::BtrfsStorageBackend;

int main(int argc, char* argv[]) {
  LOGGER->setDebugLevel (LOW_LEVEL);
  BtrfsStorageBackend backend("localhost", 1234);
  backend.Init();
  return 0;
}

