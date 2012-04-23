// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PUBLIC_FILE_LIST_H_
#define BACKUP_BACKEND_PUBLIC_FILE_LIST_H_

#include <string>

namespace backup {

class FileList {
 public:
  FileList() {}
  ~FileList() {}

  bool AddFile(std::string path);
  bool AddDirectory(std::string path);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_PUBLIC_FILE_LIST_H_
