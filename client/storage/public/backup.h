// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_CLIENT_STORAGE_PUBLIC_BACKUP_H_
#define BACKUP_CLIENT_STORAGE_PUBLIC_BACKUP_H_

#include <string>

#include "client/storage/public/file_list.h"

namespace client {

class Backup{
 public:
  Backup() {}
  virtual ~Backup() {}

  // Actually perform a backup, given a list of files to backup.  Returns true
  // if the backup is successful; false otherwise.
  // TODO: Make this asynchronous.
  virtual bool DoBackup(const FileList& filelist) = 0;

  // Open the specified backup instance represented by this instance object
  // and return back a string path representing the instance's mounted
  // location, visible from the client computer.  Implementations of this
  // function need to be OS-aware and return the appropriate style path
  // for the operating system.
  virtual const std::string OpenAndGetRestorePath() = 0;
};

}  // namespace client
#endif  // BACKUP_CLIENT_STORAGE_PUBLIC_BACKUP_H_
