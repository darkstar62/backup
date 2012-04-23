// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "client/ui/cli/cli_client.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backup::CliMain;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  CHECK_LT(1, argc) << "Must specify a command";

  // Construct the command handler.
  CliMain cli(argc, argv);
  if (!cli.Init()) {
    LOG(FATAL) << "Error initializing backend";
  }

  return cli.RunCommand();
}

