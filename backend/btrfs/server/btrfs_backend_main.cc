// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#include <signal.h>
#include <iostream>
#include <sstream>

#include "Ice/Ice.h"
#include "backend/btrfs/server/backup_impl.h"
#include "backend/btrfs/server/backup_set_impl.h"
#include "backend/btrfs/server/btrfs_backend_service_impl.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using backup::BtrfsBackendServiceImpl;
using std::ostringstream;

DEFINE_int32(port, 5827, "Port to listen on");
DEFINE_string(backend_path, "", "Path to the backup store");

namespace {

Ice::CommunicatorPtr ic;

}  // namespace

void Quit(int signal) {
  ic->shutdown();
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  // Validate the command-line arguments
  CHECK_NE("", FLAGS_backend_path) << "Must specify a valid backup store path";
  CHECK_LT(0, FLAGS_port) << "Bad port number specified";

  ic = Ice::initialize(argc, argv);

  // Register ICE server-facing object factories.
  //
  // IMPORTANT: The client-facing ICE instance MUST NOT know about server-side
  // object implementations; otherwise the dependency separation between the
  // client and server becomes fuzzy.
  backup_proto::StatusFactory::Init(ic);
  backup::BackupSetFactory::Init(ic);
  backup::BackupFactory::Init(ic);

  ostringstream host_port_stream;
  host_port_stream << "default -p " << FLAGS_port;
  Ice::ObjectAdapterPtr adapter =
      ic->createObjectAdapterWithEndpoints(
          "BtrfsBackendServiceAdapter", host_port_stream.str());

  BtrfsBackendServiceImpl* service = new BtrfsBackendServiceImpl(
      ic, FLAGS_backend_path);
  CHECK(service->Init());

  // ObjectPtr gains ownership of the service -- DON'T delete it.
  Ice::ObjectPtr object = service;
  adapter->add(
      object, ic->stringToIdentity("BtrfsBackendService"));

  LOG(INFO) << "BTRFS server listening on port " << FLAGS_port;
  signal(SIGINT, &Quit);

  adapter->activate();
  ic->waitForShutdown();
  ic->destroy();
  return 0;
}
