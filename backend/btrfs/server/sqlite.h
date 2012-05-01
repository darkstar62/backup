// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

// This file contains a wrapper for the C interface of SQLite.

#ifndef BACKUP_BACKEND_BTRFS_SERVER_SQLITE_H_
#define BACKUP_BACKEND_BTRFS_SERVER_SQLITE_H_

#include <sqlite3.h>
#include <stdint.h>
#include <string>

#include "base/macros.h"
#include "backend/btrfs/proto/status.proto.h"
#include "boost/scoped_ptr.hpp"

namespace sqlite {

class SQLiteResult;

class SQLiteDB {
 public:
  explicit SQLiteDB(const std::string& filename)
      : filename_(filename) {}
  ~SQLiteDB();

  // Initialize the DB connection.  If this returns false, the error message
  // returned by SQLite can be obtained through error_message().
  bool Init();

  // Send a query to the database.  The returned result provides methods to
  // get at the results or any errors that may have occurred.  Note that
  // ownership is *RETAINED* by this class -- do not delete the pointer.
  SQLiteResult* Query(const std::string& query);

  // Send a query not expected to return results.  If an error occurs, return
  // a StatusPtr with the given enum code and prefix reason.  The error message
  // is appended to the end of the reason.  Otherwise, kStatusOk is returned.
  backup_proto::StatusPtr QueryOrReturnStatus(
      const std::string& query, backup_proto::StatusEnum status_on_error,
      const std::string& reason);

  // Close the database.
  void Close();

  // Get the error message.
  const std::string& error_msg() const { return error_msg_; }

 private:
  // Filename of the SQLite DB.
  std::string filename_;

  // Database pointer into SQLite
  sqlite3* db_;

  // SQLite result.  This is passed to the caller, giving access to results
  // in a query.
  boost::scoped_ptr<SQLiteResult> result_;

  // Error message, if an error occurs.
  std::string error_msg_;

  DISALLOW_COPY_AND_ASSIGN(SQLiteDB);
};

// Represents a row in the result set.
class SQLiteRow {
 public:
  SQLiteRow(sqlite3* db, sqlite3_stmt* statement)
      : db_(db),
        statement_(statement),
        error_msg_(""),
        error_code_(0),
        error_(false) {
  }

  const std::string GetColumnAsString(uint32_t column);

  // Get/set the current error message.  This also modifies the error state,
  // which code can check for to determine if there was an error.
  void SetError(int32_t error_code, const std::string& error_msg) {
    error_code_ = error_code;
    error_msg_ = error_msg;
    error_ = true;
  }

  const std::string& error_msg() const { return error_msg_; }
  const int32_t error_code() const { return error_code_; }
  bool error() const { return error_; }

 private:
  // The current SQLite database.
  sqlite3* db_;

  // The current SQLite prepared statement.
  sqlite3_stmt* statement_;

  // Error states
  std::string error_msg_;
  int32_t error_code_;
  bool error_;

  DISALLOW_COPY_AND_ASSIGN(SQLiteRow);
};

// SQLiteResult encapsulates the results of a running query.  It provides
// functions for stepping through the rows of a result, or getting all
// results at once.
class SQLiteResult {
 public:
  SQLiteResult(sqlite3* db, sqlite3_stmt* statement)
      : db_(db),
        statement_(statement),
        row_(new SQLiteRow(db, statement)) {}

  ~SQLiteResult() {
    sqlite3_finalize(statement_);
  }

  // Return the next row from the query.  If there's no more, this function
  // returns NULL.  Note that ownership *REMAINS* with this class.
  SQLiteRow* NextRow();

 private:
  // The current SQLite database.
  sqlite3* db_;

  // The SQLite prepared statement.
  sqlite3_stmt* statement_;

  // The current row in the result set.
  boost::scoped_ptr<SQLiteRow> row_;

  DISALLOW_COPY_AND_ASSIGN(SQLiteResult);
};

}  // namespace sqlite
#endif  // BACKUP_BACKEND_BTRFS_SERVER_SQLITE_H_
