// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_CLIENT_UI_CLI_CLIENT_H_
#define BACKUP_CLIENT_UI_CLI_CLIENT_H_

#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "backend/public/storage_backend.h"
#include "boost/scoped_ptr.hpp"

namespace backup {

// Main class implementing functionality for the CLI interface.
class CliMain {
 public:
  CliMain(int argc, char* argv[]);
  ~CliMain();

  // Initialize the backend.  Returns whether initialization was successful.
  bool Init();

  // Run the passed-in command.  Returns EXIT_SUCCESS if the command was
  // successful, or EXIT_FAILURE if not.
  int RunCommand();

 private:
  // List the currently available backup sets.  Implements the
  // "list_backup_sets" command.
  int ListBackupSets();

  // Create a new backup set.  Implements the "create_backup_set"
  // command, taking the backup set name as the only argument.
  int CreateBackupSet();

  // Create a new incremental backup in the given backup set.  Implements the
  // "create_incremental_backup" command, taking the backup set, new backup
  // name, and size in megabytes of the new backup as arguments.
  int CreateIncrementalBackup();

  // The storage backend.
  boost::scoped_ptr<StorageBackend> backend_;

  // The ICE communicator.
  Ice::CommunicatorPtr ic_;

  // The command and the arguments passed
  std::string command_;
  std::vector<std::string> args_;
};

}  // namespace backup
#endif  // BACKUP_CLIENT_UI_CLI_CLIENT_H_
