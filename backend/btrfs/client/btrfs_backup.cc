// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/client/btrfs_backup.h"

#include <vector>

#include "backend/btrfs/proto/backup.proto.h"
#include "backend/btrfs/proto/status.proto.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusPtr;
using boost::filesystem::path;
using std::vector;

namespace backup {

BtrfsBackup::BtrfsBackup(backup_proto::BackupPrx backup_service)
    : Backup(backup_service->get_id(), backup_service->get_description()),
      backup_service_(backup_service) {
  // Populate all the rest of the Backup base-class options from the server.
  set_create_time(backup_service->get_create_time());
  set_type(backup_service->get_type() == backup_proto::kBackupTypeIncremental ?
           kBackupTypeIncremental : kBackupTypeFull);
  set_increment_of_id(backup_service->get_increment_of_id());
}

bool BtrfsBackup::Init() {
  // Initialize the server-side backup.  All file manipulation has to happen
  // there, as the client half of this probably isn't running on the same
  // machine.
  StatusPtr retval = backup_service_->Init();
  if (!retval->ok()) {
    LOG(ERROR) << "Backup init failed server-side: " << retval->ToString();
    return false;
  }

  return true;
}

bool BtrfsBackup::DoBackup(const FileList& filelist) {
  // Send the server the list of files and their sizes.  The server will send
  // back a list of definitely new files that we know we need to transmit.
  backup_proto::FileAndSizeList files_and_sizes;
  for (vector<path>::const_iterator iter = filelist.normal_files_.begin();
       iter != filelist.normal_files_.end(); ++iter) {
    path file = *iter;
    size_t size = file_size(file);
    backup_proto::FileAndSize file_and_size;
    file_and_size.filename = file.string();
    file_and_size.size = size;
    files_and_sizes.push_back(file_and_size);
  }

  backup_proto::FileList definitely_different =
      backup_service_->CheckFileSizes(files_and_sizes);

  // For all possibly identical files, grab hash chunks from 1% of the file,
  // evenly spaced, and send them to the server.  The server will respond with
  // which files are definitely different.  This is only done for files of
  // greater than 1mb.  Smaller files are transferred completely.  We send some
  // number of chunks at a time, rather than all at once.  If the server
  // responds at any time that the chunks didn't match, the file is different
  // and transferred in its entirety.

  // Start sending file contents, in order.  If the file is marked as possibly
  // identical, only send chunk hashes, until the server responds that the
  // files are different.  At that point, the server is required to begin
  // filling in skipped chunks from a file containing identical data up to
  // that point, while we start sending real data at the failure point.  Other
  // definitely different files are transferred in their entirety.
  return true;
}

}  // namespace backup
