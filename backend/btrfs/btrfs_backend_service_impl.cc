// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <fstream>
#include <vector>

#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "backend/btrfs/file_formats.pb.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using boost::filesystem::path;
using std::fstream;
using std::ios;
using std::vector;

namespace backend {

BtrfsBackendServiceImpl::BtrfsBackendServiceImpl(const std::string& path)
    : BtrfsBackendService(), path_(path) {
  // Verify the path given exists and is a directory.
  ::path fs_path(path_);
  if (!boost::filesystem::exists(fs_path) ||
      !boost::filesystem::is_directory(fs_path)) {
    LOG(FATAL) << "Specified path doesn't exist or isn't a directory.";
  }
}

BtrfsBackendServiceImpl::~BtrfsBackendServiceImpl() {
  LOG(INFO) << "Creating backup descriptor";
  path desc_path = path(path_) / "backup_descriptor.cfg";
  fstream output(desc_path.string().c_str(), ios::out | ios::binary);
  backup_descriptor_.SerializeToOstream(&output);
  output.close();
}

bool BtrfsBackendServiceImpl::Init() {
  // Check if we have a backup directory descriptor file.
  path desc_path = path(path_) / "backup_descriptor.cfg";
  if (!boost::filesystem::exists(desc_path)) {
    LOG(INFO) << "Creating backup descriptor";
    backup_descriptor_.Clear();
    fstream output(desc_path.string().c_str(), ios::out | ios::binary);
    backup_descriptor_.SerializeToOstream(&output);
    output.close();
  } else {
    LOG(INFO) << "Reading backup descriptor";
    fstream input(desc_path.string().c_str(), ios::in | ios::binary);
    backup_descriptor_.ParseFromIstream(&input);
    input.close();
  }

  return true;
}

void BtrfsBackendServiceImpl::Ping(const Ice::Current& current) {
  VLOG(3) << "Ping() requested";
  // Nothing to do here, we just return.
}

vector<BackupSetMessage> BtrfsBackendServiceImpl::EnumerateBackupSets(
    const Ice::Current& current) {
  VLOG(3) << "EnumerateBackupSets() requested";
  // Return a list of all the backup sets we're managing.
  // TODO(darkstar62): Enumerate the backup sets and actually return something
  // meaningful.
  vector<BackupSetMessage> retval;
  for (int i = 0; i < 10000; ++i) {
    BackupSetMessage msg;
    msg.name = "Fooooooooooooooooooooooooooooooooooooooooooooooooooooo";
    retval.push_back(msg);
  }
  return retval;
}

}  // namespace backend
