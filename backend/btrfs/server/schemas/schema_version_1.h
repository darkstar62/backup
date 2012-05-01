// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_SERVER_SCHEMAS_SCHEMA_VERSION_1_H_
#define BACKUP_BACKEND_BTRFS_SERVER_SCHEMAS_SCHEMA_VERSION_1_H_

#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/server/schemas/schema.h"
#include "backend/btrfs/server/sqlite.h"
#include "base/macros.h"

namespace backup {

// Version 1 schema format.
class SchemaVersion1 : public Schema {
 public:
  explicit SchemaVersion1(sqlite::SQLiteDB* db) : Schema(db) {}

  // Initialize the schema.  This detects errors in the schema and creates a new
  // schema if needed.
  virtual backup_proto::StatusPtr Init();

  // Create the database schema for a new database.
  virtual backup_proto::StatusPtr CreateNewDatabaseSchema();

 private:
  DISALLOW_COPY_AND_ASSIGN(SchemaVersion1);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_SERVER_SCHEMAS_SCHEMA_VERSION_1_H_
