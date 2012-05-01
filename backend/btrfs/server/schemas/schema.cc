// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/server/schemas/schema.h"

#include <stdint.h>
#include <set>
#include <string>

#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/schemas/schema_version_1.h"
#include "backend/btrfs/server/sqlite.h"
#include "base/string.h"
#include "glog/logging.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupInconsistent;
using backup_proto::kStatusOk;
using backup_proto::kStatusUnknownError;
using sqlite::SQLiteDB;
using sqlite::SQLiteResult;
using sqlite::SQLiteRow;
using std::set;
using std::string;

namespace backup {

StatusPtr Schema::DetectOrCreateSchema(SQLiteDB* db, Schema** schema) {
  // Get the version of the database.  If the database is new, this will return
  // the latest database schema.
  int32_t version = 0;
  StatusPtr retval = Schema::GetDatabaseVersion(db, &version);
  if (!retval->ok()) {
    return retval;
  }

  // Create a new schema object for the database.
  *schema = CreateSchema(db, version);
  if (!*schema) {
    return new StatusImpl(kStatusBackupInconsistent,
                          "Could not create schema for version " +
                          ToString(version));
  }
  return new StatusImpl(kStatusOk);
}

StatusPtr Schema::GetTables(SQLiteDB* db, set<string>* tables) {
  // Read off the tables in the database -- we should have a "schema" table
  // which describes the version of the database table we're looking at.
  SQLiteResult* result = db->Query(
      "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
  if (!result) {
    return new StatusImpl(kStatusBackupInconsistent,
                          db->error_msg());
  }

  while (SQLiteRow* row = result->NextRow()) {
    string name = row->GetColumnAsString(0);
    LOG(INFO) << "Table: " << name;
    tables->insert(name);
  }
  LOG(INFO) << "Tables: " << tables->size();
  return new StatusImpl(kStatusOk);
}

StatusPtr Schema::GetDatabaseVersion(SQLiteDB* db, int32_t* version) {
  // Read off the tables in the database -- we should have a "schema" table
  // which describes the version of the database table we're looking at.
  set<string> tables;
  StatusPtr retval = GetTables(db, &tables);
  if (!retval->ok()) {
    return retval;
  }

  // If there's no tables in the database, create a new schema for it.
  if (tables.size() == 0) {
    *version = kVersionLatest;
    return new StatusImpl(kStatusOk);
  }

  // Look for a schema table.
  if (tables.find(string("schema")) == tables.end()) {
    // No schema, this database looks to be corrupt.
    return new StatusImpl(kStatusBackupInconsistent,
                          "Missing schema table from sqlite database");
  }

  SQLiteResult* result = db->Query(
      "SELECT value FROM schema WHERE name='version';");
  if (!result) {
    return new StatusImpl(kStatusBackupInconsistent,
                          db->error_msg());
  }

  SQLiteRow* row = result->NextRow();
  if (!row) {
    return new StatusImpl(kStatusBackupInconsistent,
                          "No version field");
  } else if (row->error()) {
    return new StatusImpl(kStatusUnknownError,
                          "SQLite returned error getting version field: " +
                          row->error_msg());
  }

  *version = row->GetColumnAsInteger(0);

  // Make sure there's no more rows
  if ((row = result->NextRow()) != NULL) {
    return new StatusImpl(kStatusUnknownError,
                          "More than one row returned for version");
  }

  return new StatusImpl(kStatusOk);
}

Schema* Schema::CreateSchema(SQLiteDB* db, int32_t version) {
  switch (version) {
    case 1:
      return new SchemaVersion1(db);
      break;
    default:
      LOG(ERROR) << "Unknown schema version: " << version;
      break;
  }
  return NULL;
}

}  // namespace backup
