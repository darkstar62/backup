// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include "backend/public/file_list.h"

#include "boost/filesystem.hpp"
#include "glog/logging.h"

using boost::filesystem::file_status;
using boost::filesystem::path;

namespace backup {

bool FileList::AddEntry(path entry) {
  file_status stat = boost::filesystem::status(entry);
  switch (stat.type()) {
    case boost::filesystem::regular_file:
      if (boost::filesystem::file_size(entry) > 0) {
        VLOG(3) << "Normal file: " << entry.native();
        normal_files_.push_back(entry);
      } else {
        VLOG(3) << "Zero file: " << entry.native();
        zero_files_.push_back(entry);
      }
      break;

    case boost::filesystem::directory_file:
      VLOG(3) << "Directory: " << entry.native();
      directories_.push_back(entry);
      break;

    case boost::filesystem::symlink_file:
      VLOG(3) << "Symlink: " << entry.native();
      symlinks_.push_back(entry);
      break;

    case boost::filesystem::block_file:
    case boost::filesystem::character_file:
      VLOG(3) << "Dev node: " << entry.native();
      device_nodes_.push_back(entry);
      break;

    case boost::filesystem::fifo_file:
    case boost::filesystem::socket_file:
      VLOG(3) << "Fifo/socket: " << entry.native();
      fifos_sockets_.push_back(entry);
      break;

    default:
      LOG(FATAL) << "BUG: Don't know how to handle file type " << stat.type();
      break;
  }
}

}  // namespace backup
