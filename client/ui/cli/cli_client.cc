// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <signal.h>
#include <vector>

#include "Ice/Ice.h"
#include "client/storage/btrfs/btrfs_storage_backend.h"
#include "client/storage/public/backup_set.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using client::BackupSet;
using client::BtrfsStorageBackend;
using std::vector;

DEFINE_string(hostname, "localhost", "Host to connect to");
DEFINE_string(port, "5827", "Port to connect to");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  Ice::CommunicatorPtr ic = Ice::initialize(argc, argv);

  BtrfsStorageBackend backend(ic, FLAGS_hostname, FLAGS_port);
  if (!backend.Init()) {
    ic->destroy();
    return EXIT_FAILURE;
  }

  for (int i = 0; i < 500; ++i) {
    vector<BackupSet*> sets = backend.EnumerateBackupSets();
    LOG(INFO) << "Iteration " << i;
    for (vector<BackupSet*>::iterator iter = sets.begin();
         iter != sets.end();
         ++iter) {
      delete *iter;
    }
  }

  ic->destroy();
  return EXIT_SUCCESS;
}

