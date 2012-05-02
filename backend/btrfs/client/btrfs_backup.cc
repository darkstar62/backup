// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/btrfs/client/btrfs_backup.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "backend/btrfs/proto/backup.proto.h"
#include "backend/btrfs/proto/status.proto.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using backup_proto::StatusPtr;
using boost::filesystem::path;
using std::string;
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

  backup_proto::FileList different;
  backup_proto::FileList possibly_identical;
  StatusPtr retval = backup_service_->CheckFileSizes(
      files_and_sizes, different, possibly_identical);
  if (!retval->ok()) {
    LOG(ERROR) << "Error checking file sizes: " << retval->ToString();
    return false;
  }

  // Tell the server to mount the filesystem and get it ready to receive
  // files.
  retval = backup_service_->MountFilesystem();
  if (!retval->ok()) {
    LOG(ERROR) << "Error mounting remote filesystem: " << retval->ToString();
    return false;
  }

  // For all possibly identical files, grab hash chunks from 1% of the file,
  // evenly spaced, and send them to the server.  The server will respond with
  // which files are definitely different.  This is only done for files of
  // greater than 1mb.  Smaller files are transferred completely.  We send some
  // number of chunks at a time, rather than all at once.  If the server
  // responds at any time that the chunks didn't match, the file is different
  // and transferred in its entirety.
  // TODO(darkstar62): Implement this.
  for (backup_proto::FileList::const_iterator iter = possibly_identical.begin();
       iter != possibly_identical.end(); ++iter) {
    LOG(ERROR) << "Incremental algorithm not yet supported: " << *iter;
  }

  // Start sending file contents, in order.  If the file is marked as possibly
  // identical, only send chunk hashes, until the server responds that the
  // files are different.  At that point, the server is required to begin
  // filling in skipped chunks from a file containing identical data up to
  // that point, while we start sending real data at the failure point.  Other
  // definitely different files are transferred in their entirety.
  //
  // TODO(darkstar62): Implement sending chunks -- right now we're sending
  // the entire file.
  //
  // TODO(darkstar62): Implement sending file metadata -- right now all
  // files are created as root:root with -rw------- permissions.
  backup_proto::FileList errors;
  for (backup_proto::FileList::const_iterator iter = different.begin();
       iter != different.end(); ++iter) {
    int fd = open(iter->c_str(), O_RDONLY);
    if (fd < 0) {
      LOG(ERROR) << "Error opening " << *iter << ": " << strerror(errno);
      errors.push_back(*iter);
      continue;
    }

    retval = backup_service_->OpenFile(*iter);
    if (!retval->ok()) {
      LOG(ERROR) << "Server error opening file: " << *iter << ": "
                 << retval->ToString();
      errors.push_back(*iter);
      close(fd);
      continue;
    }

    uint64_t size = boost::filesystem::file_size(path(*iter));
    uint64_t offset = 0;
    string buffer;
    buffer.resize(512 * 1024);
    while (size > 0) {
      int bytes_read = read(fd, &buffer.at(0), 512*1024);
      CHECK_LT(-1, bytes_read) << "Don't know how to handle this!";

      retval = backup_service_->SendChunk(*iter, offset, bytes_read, buffer);
      if (!retval->ok()) {
        LOG(ERROR) << "Error sending chunk: " << retval->ToString();
        break;
      }

      size -= bytes_read;
      offset += bytes_read;
    }

    retval = backup_service_->CloseFile(*iter);
    if (!retval->ok()) {
      LOG(ERROR) << "Server error closing file: " << *iter << ": "
                 << retval->ToString();
      errors.push_back(*iter);
      close(fd);
      continue;
    }

    close(fd);
  }

  // Send empty directories.
  // TODO(darkstar62): Implement this.

  // Send empty files / special files.
  // TODO(darkstar62): Implement this.

  // Unmount the filesystem after the end of the backup.
  retval = backup_service_->UnmountFilesystem();
  if (!retval->ok()) {
    LOG(ERROR) << "Error unmounting remote filesystem: " << retval->ToString();
    return false;
  }

  // Schedule a verification run now that we're all done.
  // TODO(darkstar62): Implement this.
  return true;
}

}  // namespace backup
