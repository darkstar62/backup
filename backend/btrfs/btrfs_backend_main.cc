// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "Ice/Ice.h"
#include "backend/btrfs/btrfs_backend.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backend::BtrfsBackend;

DEFINE_int32(port, 5827, "Port to listen on");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  Ice::CommunicatorPtr ic = Ice::initialize(argc, argv);
  BtrfsBackend backend(ic, FLAGS_port);
  backend.Init();
  backend.Start();

  ic->destroy();
  return 0;
}
