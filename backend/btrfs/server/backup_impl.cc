// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Ice/Ice.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/server/backup_impl.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupCreateFailed;
using backup_proto::kStatusBackupInconsistent;
using backup_proto::kStatusOk;
using boost::filesystem::path;
using std::ostringstream;
using std::string;

namespace backup {

const std::string BackupImpl::kBackupImageName = "backup.btrfs.img";

StatusPtr BackupImpl::Init(const Ice::Current&) {
  if (initialized_) {
    return new StatusImpl(kStatusOk);
  }

  LOG(INFO) << "BackupImpl::Init";

  // Look for the backup filesystem.  If it doesn't exist, we need to create
  // it.
  path btrfs_image = path(mPath) / kBackupImageName;
  if (!boost::filesystem::exists(btrfs_image)) {
    // No filesystem image -- create one, sized appropriately (but sparsely).
    StatusPtr retval = CreateFilesystemImage();
    if (!retval->ok()) {
      return retval;
    }

    // If this is a full backup, we're done -- files will be populated from the
    // client in full.  Otherwise, we need to bring forward everything from the
    // previous backup in the form of symlinks.
    if (mType == backup_proto::kBackupTypeIncremental) {
      retval = InitializeIncrementalBackup();
      if (!retval->ok()) {
        return retval;
      }
    }
  }

  // The file exists -- make sure it's the right size.  If it isn't, that's a sure
  // sign of corruption.
  if (boost::filesystem::file_size(btrfs_image) != mSize_in_mb * 1024 * 1024) {
    ostringstream reason;
    reason << "Filesystem size doesn't match: Expected "
           << (mSize_in_mb * 1024 * 1024) << ", found "
           << boost::filesystem::file_size(btrfs_image);
    return new StatusImpl(kStatusBackupInconsistent, reason.str());
  }

  // There may be other errors in the backup, but we don't check for them here.
  // Some problems will be detected in the course of restoring, but optimally
  // a periodic scan run on the backups will detect problems.

  initialized_ = true;
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::CreateFilesystemImage() {
  path btrfs_image = path(mPath) / kBackupImageName;
  int fd = open(btrfs_image.native().c_str(), O_RDWR | O_CREAT | O_TRUNC | O_EXCL,
                S_IRUSR | S_IWUSR);
  if (fd == -1) {
    LOG(ERROR) << "Could not create backup filesystem: " << strerror(errno);
    return new StatusImpl(kStatusBackupCreateFailed,
                          "Could not create backup filesystem: " +
                          string(strerror(errno)));
  }

  if (lseek(fd, mSize_in_mb * 1024 * 1024, SEEK_SET) == -1) {
    LOG(ERROR) << "Could not resize backup filesystem: "
               << "Error " << errno << ": " << strerror(errno);
    close(fd);
    if(unlink(btrfs_image.native().c_str()) == -1) {
      LOG(ERROR) << "Cannot remove failed backup file: " << strerror(errno);
    }
    return new StatusImpl(kStatusBackupCreateFailed,
                          "Could not resize backup filesystem: " +
                          string(strerror(errno)));
  }

  close(fd);

  // Create the filesystem.  This requires the 'mkfs.btrfs' program be in
  // the current path.
  string command = "mkfs.btrfs " + btrfs_image.native();
  LOG(INFO) << "Executing command: " << command;
  int retval = system(command.c_str());
  if (retval != 0) {
    if(unlink(btrfs_image.native().c_str()) == -1) {
      LOG(ERROR) << "Cannot remove failed backup file: " << strerror(errno);
    }

    if (retval == -1) {
      LOG(ERROR) << "Error executing command: " << strerror(errno);
      return new StatusImpl(
          kStatusBackupCreateFailed,
          "Error executing command: " + string(strerror(errno)));
    } else {
      LOG(ERROR) << "Command returned status " << retval;
      return new StatusImpl(
          kStatusBackupCreateFailed,
          "Error executing command: Command returned error status");
    }
  }
}

StatusPtr BackupImpl::InitializeIncrementalBackup() {
  // TODO: Support incremental backups.
  path btrfs_image = path(mPath) / kBackupImageName;
  if(unlink(btrfs_image.native().c_str()) == -1) {
    LOG(ERROR) << "Cannot remove failed backup file: " << strerror(errno);
  }
  return new StatusImpl(
      kStatusBackupCreateFailed,
      "Incremental backups not (yet) supported");
}

}  // namespace backup
