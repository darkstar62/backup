// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_SERVER_BACKUP_SET_IMPL_H_
#define BACKUP_BACKEND_BTRFS_SERVER_BACKUP_SET_IMPL_H_

#include <string>

#include "Ice/Ice.h"
#include "backend/btrfs/server/backup_descriptor.proto.h"
#include "glog/logging.h"

namespace backup {

// Implementation of the server-side BackupSet proto class.  This class actually
// does the server-side work of managing backup sets.
class BackupSetImpl : public backup_proto::BackupSet {
 public:
  BackupSetImpl()
      : backup_proto::BackupSet(),
        initialized_(false) {}
  virtual ~BackupSetImpl();

  // Initialize the backup set descriptor and get the server-side backup
  // set ready to use client-side.
  backup_proto::StatusPtr Init(const Ice::Current&);

  // Create a backup.
  backup_proto::StatusPtr CreateBackup(
      backup_proto::BackupType type,
      const backup_proto::BackupOptions& options,
      backup_proto::BackupPrx& backup_ref,
      const Ice::Current&);

  // Enumerate the backups in the set.
  backup_proto::StatusPtr EnumerateBackups(
      backup_proto::BackupPtrList& backup_list_ref,
      const Ice::Current&);

  // Accessors for the ID.
  std::string get_id(const Ice::Current&) { return id.name; }

  // Accessors for the name.  None of the data members are available to the
  // client, so we access it via RPC accessors.
  std::string get_name(const Ice::Current&) { return mName; }
  void set_name(const std::string& name, const Ice::Current&) { mName = name; }

 private:
  // Backup set descriptor.
  backup_proto::BackupSetDescriptor descriptor_;

  // Whether we've initialied or not
  bool initialized_;
};

// BackupSet factory to help ICE initialize the objects.
class BackupSetFactory : public Ice::ObjectFactory {
 public:
  virtual Ice::ObjectPtr create(const std::string& type) {
    CHECK_EQ(backup_proto::BackupSet::ice_staticId(), type);
    return new BackupSetImpl;
  }
  virtual void destroy() {}

  static void Init(Ice::CommunicatorPtr ic) {
    ic->addObjectFactory(new BackupSetFactory,
                         backup_proto::BackupSet::ice_staticId());
  }
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_SERVER_BACKUP_SET_IMPL_H_
