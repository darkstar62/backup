// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <iostream>
#include <sstream>
#include <string>

#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"

using std::ostringstream;
using std::string;

namespace backup_proto {

string StatusImpl::ToString(const Ice::Current& current) {
  ostringstream str_reason;
  switch (id) {
    case kStatusOk:
      str_reason << "OK: ";
      break;
    case kStatusUnknownError:
      str_reason << "Unknown error: ";
      break;
    case kStatusBackupSetNotFound:
      str_reason << "Backup set not found: ";
      break;
    case kStatusBackupSetCreateFailed:
      str_reason << "Backup set create failed: ";
      break;
    default:
      LOG(FATAL) << "Internal error: status id should not be " << id;
      break;
  }

  if (reason == "") {
    str_reason << "No reason given";
  } else {
    str_reason << reason;
  }

  return str_reason.str();
}

}  // namespace backup_proto
