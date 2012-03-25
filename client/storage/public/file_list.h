// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_CLIENT_STORAGE_PUBLIC_FILE_LIST_H_
#define BACKUP_CLIENT_STORAGE_PUBLIC_FILE_LIST_H_

#include <string>

namespace client {

class FileList {
 public:
  FileList() {}
  ~FileList() {}

  bool AddFile(string path);
  bool AddDirectory(string path);
};

}  // namespace client
#endif  // BACKUP_CLIENT_STORAGE_PUBLIC_FILE_LIST_H_
