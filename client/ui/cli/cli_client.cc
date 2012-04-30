// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "backend/btrfs/client/btrfs_storage_backend.h"
#include "backend/public/backup.h"
#include "backend/public/backup_set.h"
#include "client/ui/cli/cli_client.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

DEFINE_string(backend, "invalid",
              "Storage backend to use.  Valid values are 'btrfs'.");

// BTRFS backend connection options.
DEFINE_string(btrfs_hostname, "localhost",
              "Host to connect to for backend 'btrfs'");
DEFINE_string(btrfs_port, "5827",
              "Port to connect to for backend 'btrfs'");

namespace backup {

CliMain::CliMain(int argc, char* argv[]) {
  if (FLAGS_backend == "btrfs") {
    backend_.reset(new BtrfsStorageBackend(
        argc, argv, FLAGS_btrfs_hostname, FLAGS_btrfs_port));
  } else {
    LOG(FATAL) << "Invalid backend specified: " << FLAGS_backend;
  }

  // Parse the command and the arguments
  CHECK_LT(1, argc) << "Must specify a command";
  command_ = string(argv[1]);

  for (int a = 2; a < argc; ++a) {
    args_.push_back(string(argv[a]));
  }
}

bool CliMain::Init() {
  return backend_->Init();
}

int CliMain::RunCommand() {
  if (command_ == "list_backup_sets") {
    return ListBackupSets();
  } else if (command_ == "create_backup_set") {
    return CreateBackupSet();
  } else if (command_ == "create_incremental_backup") {
    return CreateIncrementalBackup();
  } else if (command_ == "create_full_backup") {
    return CreateFullBackup();
  } else if (command_ == "list_backups") {
    return ListBackups();
  } else if (command_ == "backup") {
    return DoBackup();
  }

  LOG(ERROR) << "Invalid command: " << command_;
  return EXIT_FAILURE;
}

int CliMain::ListBackupSets() {
  vector<BackupSet*> sets = backend_->EnumerateBackupSets();
  for (vector<BackupSet*>::iterator iter = sets.begin();
       iter != sets.end();
       ++iter) {
    cout << (*iter)->id() << ": " << (*iter)->description() << endl;
    delete *iter;
  }
  return EXIT_SUCCESS;
}

int CliMain::CreateBackupSet() {
  CHECK_EQ(1, args_.size()) << "Must supply a backup set name";
  string name = args_.at(0);

  BackupSet* backup_set = NULL;
  if (!backend_->CreateBackupSet(name, &backup_set)) {
    LOG(ERROR) << "Error creating backup set: " << name;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int CliMain::CreateIncrementalBackup() {
  CHECK_EQ(3, args_.size())
      << "Must specify a backup set name, backup name, and size";

  string backup_set_id = args_.at(0);
  string backup_name = args_.at(1);
  uint64_t backup_size_mb;

  CHECK_EQ(1, sscanf(args_.at(2).c_str(), "%lu", &backup_size_mb))
      << "Third argument must be a positive integer";

  // Find the backup set.
  BackupSet* backup_set = GetBackupSet(backup_set_id);
  if (!backup_set) {
    return EXIT_FAILURE;
  }

  Backup* backup = backup_set->CreateIncrementalBackup(
      BackupOptions().set_description(backup_name)
                     .set_size_in_mb(backup_size_mb));
  if (!backup) {
    LOG(ERROR) << "Error creating incremental backup";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int CliMain::CreateFullBackup() {
  CHECK_EQ(3, args_.size())
      << "Must specify a backup set id, backup name, and size";

  string backup_set_id = args_.at(0);
  string backup_name = args_.at(1);
  uint64_t backup_size_mb;

  CHECK_EQ(1, sscanf(args_.at(2).c_str(), "%lu", &backup_size_mb))
      << "Third argument must be a positive integer";

  BackupSet* backup_set = GetBackupSet(backup_set_id);
  if (!backup_set) {
    return EXIT_FAILURE;
  }

  Backup* backup = backup_set->CreateFullBackup(
      BackupOptions().set_description(backup_name)
                     .set_size_in_mb(backup_size_mb));
  if (!backup) {
    LOG(ERROR) << "Error creating full backup";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int CliMain::ListBackups() {
  CHECK_EQ(1, args_.size()) << "Must specify a backup set id";
  string backup_set_id = args_.at(0);

  // Get the backup set -- we'll query it for the backups.
  BackupSet* backup_set = GetBackupSet(backup_set_id);
  if (!backup_set) {
    return EXIT_FAILURE;
  }

  vector<Backup*> sets = backup_set->EnumerateBackups();
  for (vector<Backup*>::iterator iter = sets.begin();
       iter != sets.end();
       ++iter) {
    cout << (*iter)->id() << ": "
         << ((*iter)->type() == kBackupTypeIncremental ?
             "incremental: " : "full: ")
         << (*iter)->description() << endl;
    delete *iter;
  }
  return EXIT_SUCCESS;
}

int CliMain::DoBackup() {
  CHECK_EQ(3, args_.size())
      << "Must specify a backup set, backup, and file list.";
  string backup_set_id = args_.at(0);
  string backup_id = args_.at(1);
  string filelist = args_.at(2);

  // Get the backup set
  BackupSet* backup_set = GetBackupSet(backup_set_id);
  if (!backup_set) {
    return EXIT_FAILURE;
  }

  // Get the backup.
  vector<Backup*> sets = backup_set->EnumerateBackups();
  Backup* backup = NULL;
  for (vector<Backup*>::iterator iter = sets.begin();
       iter != sets.end(); ++iter) {
    if ((*iter)->id() == backup_id) {
      backup = *iter;
      break;
    }
  }
  if (!backup) {
    LOG(FATAL) << "Could not find backup " << backup_id;
  }

  // Read the list of files from the filelist.
  string block;
  string data;
  int fd = open(filelist.c_str(), O_RDONLY);
  if (fd == -1) {
    LOG(FATAL) << "Could not open filelist: " << strerror(errno);
  }

  ssize_t bytes_read = 0;
  do {
    block.resize(256);
    bytes_read = read(fd, &block.at(0), 256);
    block.resize(bytes_read);
    data += block;
  } while (bytes_read);

  FileList files;
  int pos = 0;
  int old_pos = 0;
  while ((pos = data.find('\n', old_pos)) != string::npos) {
    string file = data.substr(old_pos, pos - old_pos);
    if (file.size()) {
      files.AddEntry(file);
    }
    old_pos = pos + 1;
  }
  string file = data.substr(old_pos);
  if (file.size()) {
    files.AddEntry(file);
  }

  // Start the backup.
  if (!backup->DoBackup(files)) {
    LOG(FATAL) << "Error performing backup";
  }
  return EXIT_SUCCESS;
}

BackupSet* CliMain::GetBackupSet(string id) {
  // Find the backup set.
  BackupSet* backup_set = backend_->GetBackupSet(id);
  if (!backup_set) {
    LOG(ERROR) << "Could not get backup set";
    return NULL;
  }
  return backup_set;
}

}  // namespace backup
