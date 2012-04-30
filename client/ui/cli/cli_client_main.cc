// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "client/ui/cli/cli_client.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backup::CliMain;

int main(int argc, char* argv[]) {
  google::SetUsageMessage(
      "Usage: cli_client <options> <command>\n\n"
      "Commands:\n"
      "    list_backup_sets\n"
      "        List the available backup sets.\n\n"
      "    create_backup_set <backup set name>\n"
      "        Create a backup set with the given name.\n\n"
      "    create_incremental_backup <backup set id> <backup name> "
               "<size_in_mb>\n"
      "        Create an incremental backup in the given backup set with at\n"
      "        most <size_in_mb> megabytes.\n\n"
      "    create_full_backup <backup set id> <backup name> <size_in_mb>\n"
      "        Create a full backup in the given backup set with at most\n"
      "        <size_in_mb> megabytes.\n\n"
      "    list_backups <backup set id>\n"
      "        List the backups in the given backup set.\n\n"
      "    backup <backup set id> <backup id> <path to filelist>\n"
      "        Perform a backup to the given backup ID, saving the files\n"
      "        listed in the filelist file.  Files should be line-separated.");

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

