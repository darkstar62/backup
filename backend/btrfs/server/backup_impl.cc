// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/server/backup_impl.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "Ice/Ice.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/server/sqlite.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupCreateFailed;
using backup_proto::kStatusBackupFailed;
using backup_proto::kStatusBackupInconsistent;
using backup_proto::kStatusOk;
using boost::filesystem::path;
using sqlite::SQLiteDB;
using sqlite::SQLiteResult;
using sqlite::SQLiteRow;
using std::ostringstream;
using std::set;
using std::string;

namespace backup {

const std::string BackupImpl::kBackupImageName = "backup.btrfs.img";
const std::string BackupImpl::kBackupDatabaseName = "metadata.sqlite";
const std::string BackupImpl::kBackupMountPath = "_backup_mount";

StatusPtr BackupImpl::Init(const Ice::Current&) {
  if (initialized_) {
    return new StatusImpl(kStatusOk);
  }

  // Initialize the filesystem.  This does rudamentary checks on the existing
  // filesystem, or creates a new one if one doesn't exist.
  StatusPtr retval;
  retval = InitFilesystem();
  if (!retval->ok()) {
    return retval;
  }

  // Initialize the hash database.  This contains all the chunks for each file
  // as well as all the metadata surrounding each file.
  retval = InitDatabase();
  if (!retval->ok()) {
    return retval;
  }

  initialized_ = true;
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::CheckFileSizes(
    const backup_proto::FileAndSizeList& files,
    backup_proto::FileList& different,
    backup_proto::FileList& possibly_identical,
    const Ice::Current&) {
  for (backup_proto::FileAndSizeList::const_iterator iter = files.begin();
       iter != files.end(); ++iter) {
    bool found = false;
    StatusPtr retval = schema_->FileSizeExists(iter->size, &found);
    if (!retval->ok()) {
      LOG(ERROR) << "Error finding file sizes for " << iter->size << ": "
                 << retval->ToString();
      return retval;
    }

    if (found) {
      possibly_identical.push_back(iter->filename);
    } else {
      different.push_back(iter->filename);
    }
  }
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::MountFilesystem(const Ice::Current&) {
  // Mounting the filesystem requires privileges, so if we don't have them,
  // these calls will fail.
  path btrfs_image = path(mPath) / kBackupImageName;
  path btrfs_mount = path(mPath) / kBackupMountPath;

  // Create the mount directory
  // TODO(darkstar62): This form throws an exception if an error occurs --
  // we should use the error-code version instead.
  if (!boost::filesystem::create_directory(btrfs_mount)) {
    return new StatusImpl(kStatusBackupFailed,
                          "Failed to create mount directory");
  }

  // TODO(darkstar62): Yes, this is hacky.  But it *does* insulate us from
  // kernel changes.  Of course, this only works in Linux...
  string command =
      "mount -t btrfs -o loop,compress-force " + btrfs_image.native() + " " +
      btrfs_mount.native();
  LOG(INFO) << "Executing command: " << command;
  int retval = system(command.c_str());
  if (retval != 0) {
    int retval_rmdir = rmdir(btrfs_mount.native().c_str());
    if (retval_rmdir < 0) {
      LOG(ERROR) << "Could not delete directory " << btrfs_mount.native()
                 << ": " << strerror(errno);
    }
    return new StatusImpl(kStatusBackupFailed,
                          "Failed to mount filesystem, check server logs.");
  }

  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::UnmountFilesystem(const Ice::Current&) {
  // Unmounting the filesystem requires privileges, so if we don't have them,
  // these calls will fail.
  path btrfs_image = path(mPath) / kBackupImageName;
  path btrfs_mount = path(mPath) / kBackupMountPath;

  // TODO(darkstar62): Yes, this is hacky.  But it *does* insulate us from
  // kernel changes.  Of course, this only works in Linux...
  string command = "umount " + btrfs_mount.native();
  LOG(INFO) << "Executing command: " << command;
  int retval = system(command.c_str());
  if (retval != 0) {
    return new StatusImpl(kStatusBackupFailed,
                          "Failed to unmount filesystem, check server logs.");
  }

  int retval_rmdir = rmdir(btrfs_mount.native().c_str());
  if (retval_rmdir < 0) {
    LOG(ERROR) << "Could not delete directory " << btrfs_mount.native()
               << ": " << strerror(errno);
    return new StatusImpl(
        kStatusBackupFailed,
        "Failed to remove mountpoint directory: " + string(strerror(errno)));
  }

  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::OpenFile(
    const string& filename, const Ice::Current&) {
  // TODO(darkstar62): Files need to be in a hash so we can write to more
  // than one at a time.
  path file_path = path(mPath) / kBackupMountPath / filename;
  path directory = file_path.parent_path();

  if (!boost::filesystem::create_directories(directory)) {
    if (!boost::filesystem::exists(directory) ||
        !boost::filesystem::is_directory(directory)) {
      // We couldn't create the directory.
      return new StatusImpl(
          kStatusBackupFailed,
          "Failed to create directory for file: " + filename);
    }
  }

  fd_ = open(file_path.native().c_str(),
             O_RDWR | O_CREAT | O_TRUNC | O_EXCL,
             S_IRUSR | S_IWUSR);
  if (fd_ == -1) {
    return new StatusImpl(
        kStatusBackupFailed,
        "Failed to open file " + filename + ": " + strerror(errno));
  }
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::CloseFile(
    const string& filename, const Ice::Current&) {
  int ret = close(fd_);
  if (ret == -1) {
    return new StatusImpl(
        kStatusBackupFailed,
        "Failed to close file " + filename + ": " + strerror(errno));
  }
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::SendChunk(
    const string& filename, Ice::Long offset, Ice::Long size,
    const string& buffer, const Ice::Current&) {
  int ret = lseek(fd_, offset, SEEK_SET);
  if (ret == -1) {
    return new StatusImpl(
        kStatusBackupFailed,
        "Failed to seek in file " + filename + ": " + strerror(errno));
  }

  ret = write(fd_, &buffer.at(0), size);
  if (ret == -1) {
    return new StatusImpl(
        kStatusBackupFailed,
        "Failed to write chunk in file " + filename + ": " + strerror(errno));
  }

  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::InitFilesystem() {
  // Look for the backup filesystem.  If it doesn't exist, we need to create
  // it.
  path btrfs_image = path(mPath) / kBackupImageName;
  if (!boost::filesystem::exists(btrfs_image)) {
    // No filesystem image -- create one, sized appropriately (but sparsely).
    StatusPtr retval = CreateFilesystemImage();
    if (!retval->ok()) {
      return retval;
    }

    // We're done at the end of this, whether it's an incremental backup or not.
    // Symlinks from previous backups are applied *during* the backup, as the
    // file linked to may not have the same path as previously due to copies or
    // renames.
  }

  // The file exists -- make sure it's the right size.  If it isn't, that's a
  // sure sign of corruption.
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
  return new StatusImpl(kStatusOk);
}

StatusPtr BackupImpl::InitDatabase() {
  // Connect to the database
  path db_path = path(mPath) / kBackupDatabaseName;
  SQLiteDB* db = new SQLiteDB(db_path.native());
  if (!db->Init()) {
    delete db;
    return new StatusImpl(kStatusBackupInconsistent,
                          db->error_msg());
  }

  // Get the database schema for the database.  This has the effect of creating
  // the schema too if the database is new.  Note that on success, ownership of
  // the database is transferred to the schema.
  Schema* schema = NULL;
  StatusPtr retval = Schema::DetectOrCreateSchema(db, &schema);
  if (!retval->ok()) {
    delete db;
    return retval;
  }

  // Initialize the schema.  On a new database, this creates the empty schema.
  // Existing databases are checked quickly for consistency.
  retval = schema->Init();
  if (!retval->ok()) {
    delete schema;
    return retval;
  }

  schema_.reset(schema);

  return retval;
}

StatusPtr BackupImpl::CreateFilesystemImage() {
  path btrfs_image = path(mPath) / kBackupImageName;
  int fd = open(btrfs_image.native().c_str(),
                O_RDWR | O_CREAT | O_TRUNC | O_EXCL,
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
    if (unlink(btrfs_image.native().c_str()) == -1) {
      LOG(ERROR) << "Cannot remove failed backup file: " << strerror(errno);
    }
    return new StatusImpl(kStatusBackupCreateFailed,
                          "Could not resize backup filesystem: " +
                          string(strerror(errno)));
  }

  // This bogus write here is necessary to actually get the file to the
  // right size.  An lseek() should be enough, but seems to not be.
  int foo = 1;
  lseek(fd, -sizeof(foo), SEEK_CUR);
  write(fd, &foo, sizeof(foo));
  close(fd);

  // Create the filesystem.  This requires the 'mkfs.btrfs' program be in
  // the current path.
  string command = "mkfs.btrfs " + btrfs_image.native();
  LOG(INFO) << "Executing command: " << command;
  int retval = system(command.c_str());
  if (retval != 0) {
    if (unlink(btrfs_image.native().c_str()) == -1) {
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
  return new StatusImpl(kStatusOk);
}

}  // namespace backup
