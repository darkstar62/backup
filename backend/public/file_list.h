// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_PUBLIC_FILE_LIST_H_
#define BACKUP_BACKEND_PUBLIC_FILE_LIST_H_

#include <vector>

#include "boost/filesystem.hpp"

namespace backup {

struct FileList {
  // Add an entry to the file list.  The type of the entry is automatically
  // determined and stat'd in the way appropriate for it to support things
  // like symlinks, device nodes, named fifos, etc.
  bool AddEntry(boost::filesystem::path entry);

  // Various categories of files and directories.  Backends can handle these
  // differently as needed, and most of them don't need hashing.
  std::vector<boost::filesystem::path> symlinks_;
  std::vector<boost::filesystem::path> device_nodes_;
  std::vector<boost::filesystem::path> fifos_sockets_;
  std::vector<boost::filesystem::path> directories_;
  std::vector<boost::filesystem::path> zero_files_;
  std::vector<boost::filesystem::path> normal_files_;
};

}  // namespace backup
#endif  // BACKUP_BACKEND_PUBLIC_FILE_LIST_H_
