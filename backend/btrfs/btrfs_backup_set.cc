// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <string>

#include "backend/btrfs/btrfs_backup_set.h"
#include "backend/btrfs/status.proto.h"
#include "backend/public/backup.h"
#include "backend/public/backup_set.h"
#include "boost/filesystem.hpp"

using backup_proto::StatusImpl;
using backup_proto::StatusPtr;
using backup_proto::kStatusBackupSetNotFound;
using backup_proto::kStatusOk;
using std::string;

namespace backup {

bool BtrfsBackupSet::Init() {
  // FIXME: The BackupSet object is a *CLIENT* side object, not server-side,
  // so we can't be doing filesystem operations here -- instead we have to
  // make remote calls to the server backend on our behalf, or this info has
  // to be provided to us.
  //
  // Alternatively, if this object was a serializable ICE object, some of
  // the functions implemented here could be local while others are remote.
  // Yeah, that blurs the lines quite a bit, but it could keep the
  // implementation details of everything where it belongs, rather than
  // cramming it into the backend service implementation.
  StatusPtr retval = server_set_->Init();
  if (!retval->ok()) {
    LOG(ERROR) << "Error initializing backup set on server: "
               << retval->ToString();
    return false;
  }

  return true;
}

std::vector<Backup*> BtrfsBackupSet::EnumerateBackups() {
  // The backups are stored in the backup descriptor for the backup set.
  return std::vector<Backup*>();
}

Backup* BtrfsBackupSet::CreateIncrementalBackup(const BackupOptions& options) {
  string description = options.description();
  uint64_t size_mb = options.size_in_mb();

  // Creating an incremental backup requires a previous backup.  Though it's
  // possible to create an incremental backup using any backup increment, it's
  // also kinda pointless to do it from one in the middle.  So to simplify
  // things, we use the last created backup as the base.

  return NULL;
}

Backup* BtrfsBackupSet::CreateFullBackup(const BackupOptions& options) {
  return NULL;
}

////////////////////////////////////////

StatusPtr BackupSetServerImpl::Init(const Ice::Current& /* current */) {
  // Look for our backup set -- it'll be in the path given to us by the
  // backend.
  boost::filesystem::path set_path(mPath);
  if (!boost::filesystem::exists(set_path)) {
    LOG(ERROR) << "Backup set " << mName << " failed to init: "
               << "No such directory: " << mPath;
    return new StatusImpl(kStatusBackupSetNotFound,
                          "No such directory: " + mPath);
  }
  if (!boost::filesystem::is_directory(set_path)) {
    LOG(ERROR) << "Backup set " << mName << " failed to init: "
               << "Not a directory: " << mPath;
    return new StatusImpl(kStatusBackupSetNotFound,
                          "Not a directory: " + mPath);
  }

  // Try and find a backup set descriptor.  If there is none, we start
  // with an empty one.
  return new StatusImpl(kStatusOk);
}

}  // namespace backup
