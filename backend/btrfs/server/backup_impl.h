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

  // Get/set the unique ID for this backup.
  virtual std::string get_id(const Ice::Current&) { return mId.name; }
  virtual Ice::Identity get_id_as_identity(const Ice::Current&) { return mId; }
  virtual void set_id(const std::string& id, const Ice::Current&) {
    mId.name = id;
  }

  // Get/set the creation time for this backup.
  virtual long get_create_time(const Ice::Current&) { return mCreate_time; }
  virtual void set_create_time(long time, const Ice::Current&) {
    mCreate_time = time;
  }

  // Get/set the description of this backup.
  virtual std::string get_description(const Ice::Current&) {
    return mDescription;
  }
  virtual void set_description(const std::string& desc, const Ice::Current&) {
    mDescription = desc;
  }

  // Get/set the type of the backup.
  virtual backup_proto::BackupType get_type(const Ice::Current&) {
    return mType;
  }
  virtual void set_type(backup_proto::BackupType type, const Ice::Current&) {
    mType = type;
  }

  // Get/set the size of the backup in MB.
  virtual long get_size_in_mb(const Ice::Current&) { return mSize_in_mb; }
  virtual void set_size_in_mb(const long size, const Ice::Current&) {
    mSize_in_mb = size;
  }

  // Get/set the ID of the backup this backup is an increment of.
  virtual std::string get_increment_of_id(const Ice::Current&) {
    return mIncrement_of_id.name;
  }
  virtual void set_increment_of_id(const std::string& id, const Ice::Current&) {
    mIncrement_of_id.name = id;
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
