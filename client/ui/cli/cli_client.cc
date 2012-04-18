// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <signal.h>
#include <iostream>
#include <string>
#include <vector>

#include "Ice/Ice.h"
#include "boost/scoped_ptr.hpp"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "client/storage/public/backup_set.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using boost::scoped_ptr;
using client::BackupSet;
using client::BtrfsStorageBackend;
using std::cout;
using std::endl;
using std::string;
using std::vector;

DEFINE_string(hostname, "localhost", "Host to connect to");
DEFINE_string(port, "5827", "Port to connect to");

scoped_ptr<BtrfsStorageBackend> storage_backend;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  CHECK_LT(1, argc) << "Must specify a command";

  Ice::CommunicatorPtr ic = Ice::initialize(argc, argv);
  storage_backend.reset(new BtrfsStorageBackend(ic, FLAGS_hostname, FLAGS_port));
  if (!storage_backend->Init()) {
    ic->destroy();
    return EXIT_FAILURE;
  }

  // Process the command that was passed.
  string command(argv[1]);
  if (command == "list_backup_sets") {
    vector<BackupSet*> sets = storage_backend->EnumerateBackupSets();
    for (vector<BackupSet*>::iterator iter = sets.begin();
         iter != sets.end();
         ++iter) {
      cout << (*iter)->description() << endl;
      delete *iter;
    }
  } else if (command == "create_backup_set") {
    CHECK_EQ(3, argc) << "Must supply a backup set name";
    string name(argv[2]);
    BackupSet* backup_set = NULL;
    if (!storage_backend->CreateBackupSet(name, &backup_set)) {
      LOG(ERROR) << "Error creating backup set: " << name;
    }
  } else {
    ic->destroy();
    LOG(FATAL) << "Invalid command: " << command;
  }

  ic->destroy();
  return EXIT_SUCCESS;
}

