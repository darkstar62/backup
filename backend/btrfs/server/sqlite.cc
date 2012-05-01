// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/server/sqlite.h"

#include <sqlite3.h>
#include <string>

#include "base/string.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "glog/logging.h"

using backup_proto::StatusEnum;
using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusOk;
using std::string;

namespace sqlite {

SQLiteDB::~SQLiteDB() {
  Close();
}

bool SQLiteDB::Init() {
  sqlite3* db;
  char* error_msg;
  int rc;

  rc = sqlite3_open(filename_.c_str(), &db);
  if (rc) {
    error_msg_ = sqlite3_errmsg(db);
    sqlite3_close(db);
    return false;
  }

  db_ = db;
  return true;
}

SQLiteResult* SQLiteDB::Query(const string& query) {
  sqlite3_stmt* statement;
  int rc = sqlite3_prepare(
      db_, query.c_str(), query.size(), &statement, NULL);
  if (rc) {
    error_msg_ = sqlite3_errmsg(db_);
    return NULL;
  }

  SQLiteResult* result = new SQLiteResult(db_, statement);
  return result;
}

StatusPtr SQLiteDB::QueryOrReturnStatus(
    const string& query, StatusEnum status_on_error, const string& reason) {
  SQLiteResult* result = Query(query);
  SQLiteRow* row = result->NextRow();
  if (row) {
    // An error occurred, we shouldn't have a row.
    return new StatusImpl(status_on_error, reason + row->error_msg());
  }

  return new StatusImpl(kStatusOk);
}

void SQLiteDB::Close() {
  if (db_) {
    sqlite3_close(db_);
    db_ = NULL;
  }
}

///////////////////////////////////////////

const string SQLiteRow::GetColumnAsString(uint32_t column) {
  return ToString(sqlite3_column_text(statement_, column));
}

///////////////////////////////////////////

SQLiteRow* SQLiteResult::NextRow() {
  int rc = sqlite3_step(statement_);
  if (rc == SQLITE_ROW) {
    return row_.get();
  } else if (rc == SQLITE_MISUSE) {
    LOG(FATAL) << "SQLite returned SQLITE_MISUSE";
  } else if (rc == SQLITE_DONE) {
    return NULL;
  } else {
    row_->SetError(rc, sqlite3_errmsg(db_));
    return row_.get();
  }
  return NULL;
}

}  // namespace sqlite
