// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/server/schemas/schema_version_1.h"

#include <set>
#include <string>
#include <vector>

#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/sqlite.h"
#include "base/string.h"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusOk;
using backup_proto::kStatusBackupCreateFailed;
using backup_proto::kStatusBackupInconsistent;
using std::set;
using std::string;
using std::vector;

namespace backup {

StatusPtr SchemaVersion1::Init() {
  // Get the list of tables -- we should have three.  If we have less than that,
  // we probably have a corrupt database.  Unless, of course, we have zero
  // tables -- then we have a new database that we should apply the schema to.
  set<string> tables;
  StatusPtr retval = Schema::GetTables(db(), &tables);
  if (!retval->ok()) {
    return retval;
  }

  if (tables.size() == 0) {
    // New database, create the new schema.
    return CreateNewDatabaseSchema();
  } else if (tables.size() != 3) {
    return new StatusImpl(kStatusBackupInconsistent,
                          "Incorrect number of tables; expected 3, got " +
                          ToString(tables.size()));
  }

  // We have the right number of tables, assume we're good.
  return new StatusImpl(kStatusOk);
}

StatusPtr SchemaVersion1::CreateNewDatabaseSchema() {
  StatusPtr retval = db()->QueryOrReturnStatus(
      "CREATE TABLE schema (name TEXT PRIMARY KEY, value TEXT);",
      kStatusBackupCreateFailed, "Error occurred creating schema table: ");
  if (!retval->ok()) {
    return retval;
  }

  // Note, there's three inserts here rather than one, as SQLite < 3.7.11
  // doesn't support compound insert.
  vector<string> schema_inserts;
  schema_inserts.push_back("('version', '1')");
  schema_inserts.push_back("('next_file_id', '0')");
  schema_inserts.push_back("('next_chunk_id', '0')");

  for (vector<string>::iterator iter = schema_inserts.begin();
       iter != schema_inserts.end(); ++iter) {
    retval = db()->QueryOrReturnStatus(
        "INSERT INTO schema (name, value) VALUES " + *iter,
        kStatusBackupCreateFailed, "Error occurred adding " + *iter + ": ");
    if (!retval->ok()) {
      return retval;
    }
  }

  retval = db()->QueryOrReturnStatus(
      "CREATE TABLE files ("
          "file_id INT PRIMARY KEY,"
          "name TEXT,"
          "full_path TEXT,"
          "size INT,"
          "type TEXT,"
          "chunk_size INT,"
          "num_chunks INT,"
          "first_chunk_id INT);",
      kStatusBackupCreateFailed, "Error occurred creating files table: ");
  if (!retval->ok()) {
    return retval;
  }

  retval = db()->QueryOrReturnStatus(
      "CREATE TABLE chunks ("
          "chunk_id INT PRIMARY KEY,"
          "chunk_number INT,"
          "file_id INT,"
          "checksum TEXT)",
      kStatusBackupCreateFailed, "Error occurred creating chunks table: ");
  if (!retval->ok()) {
    return retval;
  }

  return new StatusImpl(kStatusOk);
}

}  // namespace backup
