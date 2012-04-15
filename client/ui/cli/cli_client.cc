// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <signal.h>

#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using client::BtrfsStorageBackend;

DEFINE_string(hostname, "localhost", "Host to connect to");
DEFINE_int32(port, 5827, "Port to connect to");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  BtrfsStorageBackend backend(FLAGS_hostname, FLAGS_port);
  backend.Init();
  return 0;
}

