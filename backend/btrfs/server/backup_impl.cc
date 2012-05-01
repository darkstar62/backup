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

StatusPtr BackupImpl::Init(const Ice::Current&) {
  if (initialized_) {
    return new StatusImpl(kStatusOk);
  }

  LOG(INFO) << "BackupImpl::Init";

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

backup_proto::FileList BackupImpl::CheckFileSizes(
    const backup_proto::FileAndSizeList& files,
    const Ice::Current&) {
  // TODO(darkstar62): Implement this
  backup_proto::FileList definitely_new;
  return definitely_new;
}

backup_proto::StatusPtr BackupImpl::InitFilesystem() {
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
  LOG(INFO) << "BackupImpl::InitDatabase";

  // Connect to the database
  path db_path = path(mPath) / kBackupDatabaseName;
  SQLiteDB db(db_path.native());
  if (!db.Init()) {
    return new StatusImpl(kStatusBackupInconsistent,
                          db.error_msg());
  }

  // Read off the tables in the database -- we should have a "schema" table
  // which describes the version of the database table we're looking at.
  SQLiteResult* result = db.Query(
      "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
  if (!result) {
    return new StatusImpl(kStatusBackupInconsistent,
                          db.error_msg());
  }

  set<string> tables;
  while (SQLiteRow* row = result->NextRow()) {
    string name = row->GetColumnAsString(0);
    LOG(INFO) << "Table: " << name;
    tables.insert(name);
  }
  LOG(INFO) << "Tables: " << tables.size();

  // If there's no tables in the database, create a new schema for it.
  if (tables.size() == 0) {
    // New database, go create the schema.
    return CreateNewDatabaseSchema(&db);
  }

  // Look for a schema table.
  if (tables.find(string("schema")) == tables.end()) {
    // No schema, this database looks to be corrupt.
    return new StatusImpl(kStatusBackupInconsistent,
                          "Missing schema table from sqlite database");
  }

  // FIXME(darkstar62, #7): How do we deal with different schema versions?
  return new StatusImpl(kStatusOk);
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

StatusPtr BackupImpl::CreateNewDatabaseSchema(SQLiteDB* db) {
  StatusPtr retval = db->QueryOrReturnStatus(
      "CREATE TABLE schema (name TEXT PRIMARY KEY, value TEXT);",
      kStatusBackupCreateFailed, "Error occurred creating schema table: ");
  if (!retval->ok()) {
    return retval;
  }

  retval = db->QueryOrReturnStatus(
      "INSERT INTO schema (name, value) VALUES ('version', '1')",
      kStatusBackupCreateFailed, "Error occurred adding version: ");
  if (!retval->ok()) {
    return retval;
  }

  return new StatusImpl(kStatusOk);
}

}  // namespace backup
