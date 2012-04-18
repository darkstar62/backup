// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Ice/Ice.h"
#include "backend/btrfs/btrfs_backend_service.proto.h"
#include "backend/btrfs/btrfs_backend_service_impl.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"

using boost::filesystem::path;
using std::fstream;
using std::ios;
using std::ostringstream;
using std::string;
using std::vector;

namespace backend {

BtrfsBackendServiceImpl::BtrfsBackendServiceImpl(Ice::CommunicatorPtr ic,
                                                 const std::string& path)
    : BtrfsBackendService(),
      path_(path),
      ic_(ic) {
  // Verify the path given exists and is a directory.
  ::path fs_path(path_);
  if (!boost::filesystem::exists(fs_path) ||
      !boost::filesystem::is_directory(fs_path)) {
    LOG(FATAL) << "Specified path doesn't exist or isn't a directory.";
  }

  backup_descriptor_.next_id = 0;
}

BtrfsBackendServiceImpl::~BtrfsBackendServiceImpl() {
  LOG(INFO) << "Creating backup descriptor";
  path desc_path = path(path_) / "backup_descriptor.cfg";

  // Convert the list of backup sets to a data stream we can write.
  Ice::OutputStreamPtr out = Ice::createOutputStream(ic_);
  out->write(backup_descriptor_);

  vector<uint8_t> out_stream;
  out->finished(out_stream);

  fstream output(desc_path.string().c_str(), ios::out | ios::binary);
  output.write(reinterpret_cast<const char*>(&out_stream.at(0)), out_stream.size());
  output.close();
}

bool BtrfsBackendServiceImpl::Init() {
  // Check if we have a backup directory descriptor file.
  path desc_path = path(path_) / "backup_descriptor.cfg";
  if (boost::filesystem::exists(desc_path)) {
    LOG(INFO) << "Reading backup descriptor";
    fstream input(desc_path.string().c_str(), ios::in | ios::binary);
    vector<uint8_t> in_stream;
    in_stream.resize(boost::filesystem::file_size(desc_path));
    input.read(reinterpret_cast<char*>(&in_stream.at(0)), in_stream.size());
    input.close();

    Ice::InputStreamPtr in = Ice::createInputStream(ic_, in_stream);
    in->read(backup_descriptor_);
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
  vector<BackupSetMessage> retval;
  for (int i = 0; i < backup_descriptor_.backup_sets.size(); ++i) {
    BackupSetMessage msg = backup_descriptor_.backup_sets.at(i);
    retval.push_back(msg);
  }
  return retval;
}

bool BtrfsBackendServiceImpl::CreateBackupSet(
    const string& name, BackupSetMessage& set_ref, const Ice::Current& current) {
  BackupSetMessage proto_set;
  proto_set.name = name;
  proto_set.id = backup_descriptor_.next_id++;
  backup_descriptor_.backup_sets.push_back(proto_set);

  set_ref = proto_set;
  return true;
}

}  // namespace backend
