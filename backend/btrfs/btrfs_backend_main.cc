// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/btrfs_backend.h"

using backend::BtrfsBackend;

int main(int argc, char* argv[]) {
  BtrfsBackend backend(1234);
  backend.Init();
  backend.Start();
  return 0;
}
