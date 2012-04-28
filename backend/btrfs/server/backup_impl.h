// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_SERVER_BACKUP_IMPL_H_
#define BACKUP_BACKEND_BTRFS_SERVER_BACKUP_IMPL_H_

#include "Ice/Ice.h"
#include "backend/btrfs/proto/backup.proto.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"

namespace backup {

// Implementation of the ICE Backup object.  This lets the client half
// enact server-side actions on its behalf via RPC.
class BackupImpl : public backup_proto::Backup {
 public:
  virtual ~BackupImpl() {}

  // Initialize the backup.  This includes creating the BTRFS image
  // and initializing it if it doesn't exist already.
  virtual backup_proto::StatusPtr Init(const Ice::Current&);

  // Various accessors for the client.
  virtual std::string get_id(const Ice::Current&) { return id.name; }
  virtual long get_create_time(const Ice::Current&) { return create_time; }
  virtual std::string get_description(const Ice::Current&) {
    return description;
  }
  virtual backup_proto::BackupType get_type(const Ice::Current&) {
    return type;
  }
  virtual long get_size_in_mb(const Ice::Current&) { return size_in_mb; }
  virtual std::string get_increment_of_id(const Ice::Current&) {
    return increment_of_id.name;
  }
};

// Backup factory to help ICE initialize the objects.
class BackupFactory : public Ice::ObjectFactory {
 public:
  virtual Ice::ObjectPtr create(const std::string& type) {
    CHECK_EQ(backup_proto::Backup::ice_staticId(), type);
    return new BackupImpl;
  }
  virtual void destroy() {}

  static void Init(Ice::CommunicatorPtr ic) {
    ic->addObjectFactory(new BackupFactory,
                         backup_proto::Backup::ice_staticId());
  }
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_SERVER_BACKUP_IMPL_H_
