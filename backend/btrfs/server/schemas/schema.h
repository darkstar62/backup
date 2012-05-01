// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_SERVER_SCHEMAS_SCHEMA_H_
#define BACKUP_BACKEND_BTRFS_SERVER_SCHEMAS_SCHEMA_H_

#include <set>
#include <string>

#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/server/sqlite.h"
#include "base/macros.h"
#include "boost/scoped_ptr.hpp"

namespace backup {

// This is the base-class Schema class, which is responsible for detecting the
// schema version of a database and selecting the correct Schema version for
// it.  It also serves as the latest API for schemas -- not all schema versions
// support all features, so each API function returns status which will
// indicate whether a feature is supported or not.
class Schema {
 public:
  explicit Schema(sqlite::SQLiteDB* db) : db_(db) {}
  virtual ~Schema() {}

  ////////////////////////////////////
  // Schema Interface Functions
  ////////////////////////////////////

  // Initialize the schema.  This detects errors in the schema and creates a new
  // schema if needed.
  virtual backup_proto::StatusPtr Init() = 0;

  // Create the database schema for a new database.
  virtual backup_proto::StatusPtr CreateNewDatabaseSchema() = 0;

  // Look for any file with the given size.  Returns true in *found if there
  // is, or false otherwise.  Status indicates any errors.
  virtual backup_proto::StatusPtr FileSizeExists(
      uint64_t size, bool* found) = 0;

  //////////////////////////////////////////
  // Schema Detection / Creation Functions
  //////////////////////////////////////////

  // Try to detect the schema version of the passed database, or create a new
  // schema if needed.  The passed schema pointer is populated with a new
  // instance of the Schema subclass corresponding to the database version
  // if successful.
  static backup_proto::StatusPtr DetectOrCreateSchema(
      sqlite::SQLiteDB* db, Schema** schema);

  // Get the tables in the specified database, returning them in the supplied
  // set.
  static backup_proto::StatusPtr GetTables(sqlite::SQLiteDB* db,
                                           std::set<std::string>* tables);

  // Get the database schema version from the database.  On success, the
  // version is returned into the version pointer.
  static backup_proto::StatusPtr GetDatabaseVersion(sqlite::SQLiteDB* db,
                                                    int32_t* version);

  // Create a new schema object for the given version.  If the version is
  // invalid, this returns NULL.
  static Schema* CreateSchema(sqlite::SQLiteDB* db, int32_t version);

  //////////////////////////////////////////
  // Other functions.
  //////////////////////////////////////////

  // Accessor for the database, for subclasses.
  sqlite::SQLiteDB* db() { return db_.get(); }

 private:
  // Current (latest) version.
  static const uint32_t kVersionLatest = 1;

  // The SQLite database this schema is applied to.
  boost::scoped_ptr<sqlite::SQLiteDB> db_;

  DISALLOW_COPY_AND_ASSIGN(Schema);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_SERVER_SCHEMAS_SCHEMA_H_
