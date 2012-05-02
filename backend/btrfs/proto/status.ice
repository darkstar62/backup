// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>
#ifndef BACKUP_BACKEND_BTRFS_STATUS_ICE_
#define BACKUP_BACKEND_BTRFS_STATUS_ICE_

module backup_proto {

enum StatusEnum {
  kStatusOk,
  kStatusUnknownError,
  kStatusBackupSetNotFound,
  kStatusBackupSetCreateFailed,
  kStatusBackupCreateFailed,
  kStatusBackupInconsistent,
  kStatusQueryFailed,
  kStatusBackupFailed
};

class Status {
  // Return whether the status object represents an OK response or not.  If the
  // status is OK, this returns true; otherwise false is returned (i.e. anything
  // not kStatusOK is false).
  bool ok();

  // Convert the status object to a string representation.  Useful for logging.
  string ToString();

  ["protected"] StatusEnum id;
  ["protected"] string reason;
};

};  // module backup_proto

#endif  // BACKUP_BACKEND_BTRFS_STATUS_ICE_
