// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BACKEND_BTRFS_SERVER_BACKUP_IMPL_H_
#define BACKUP_BACKEND_BTRFS_SERVER_BACKUP_IMPL_H_

#include <string>

#include "Ice/Ice.h"
#include "backend/btrfs/proto/backup.proto.h"
#include "backend/btrfs/proto/status.proto.h"
#include "backend/btrfs/proto/status_impl.h"
#include "backend/btrfs/server/schemas/schema.h"
#include "base/macros.h"
#include "boost/scoped_ptr.hpp"

namespace sqlite {
class SQLiteDB;
}  // namespace sqlite

namespace backup {

// Implementation of the ICE Backup object.  This lets the client half
// enact server-side actions on its behalf via RPC.
class BackupImpl : public backup_proto::Backup {
 public:
  BackupImpl()
      : backup_proto::Backup(),
        initialized_(false) {
  }
  virtual ~BackupImpl() {}

  // Initialize the backup.  This includes creating the BTRFS image
  // and initializing it if it doesn't exist already.
  virtual backup_proto::StatusPtr Init(const Ice::Current&);

  // Given a list of files and their sizes, figure out which file sizes we
  // haven't seen thus far and return the names.  This forms a set of files
  // which we know we need to transfer fully.
  virtual backup_proto::StatusPtr CheckFileSizes(
      const backup_proto::FileAndSizeList& files,
      backup_proto::FileList& different,
      backup_proto::FileList& possibly_identical,
      const Ice::Current&);

  // Get/set the unique ID for this backup.
  virtual std::string get_id(const Ice::Current&) { return mId.name; }
  virtual Ice::Identity get_id_as_identity(const Ice::Current&) { return mId; }
  virtual void set_id(const std::string& id, const Ice::Current&) {
    mId.name = id;
  }

  // Get/set the creation time for this backup.
  virtual int64_t get_create_time(const Ice::Current&) { return mCreate_time; }
  virtual void set_create_time(int64_t time, const Ice::Current&) {
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
  virtual int64_t get_size_in_mb(const Ice::Current&) { return mSize_in_mb; }
  virtual void set_size_in_mb(const int64_t size, const Ice::Current&) {
    mSize_in_mb = size;
  }

  // Get/set the ID of the backup this backup is an increment of.
  virtual std::string get_increment_of_id(const Ice::Current&) {
    return mIncrement_of_id.name;
  }
  virtual void set_increment_of_id(const std::string& id, const Ice::Current&) {
    mIncrement_of_id.name = id;
  }

  // Get/set the full path for the backup.
  virtual std::string get_path(const Ice::Current&) {
    return mPath;
  }
  virtual void set_path(const std::string& path, const Ice::Current&) {
    mPath = path;
  }

 private:
  static const std::string kBackupImageName;
  static const std::string kBackupDatabaseName;

  // Initialize the filesystem.
  backup_proto::StatusPtr InitFilesystem();

  // Initialize the database.
  backup_proto::StatusPtr InitDatabase();

  // Create the initially empty BTRFS filesystem.  This is the first step of
  // initializing the backup, whether it's full or otherwise.
  backup_proto::StatusPtr CreateFilesystemImage();

  // Whether we've initialized or not.  This is called every time a client-side
  // backup is created, so we don't want to initialize the class more than once.
  bool initialized_;

  // Database schema connected with our database.
  boost::scoped_ptr<Schema> schema_;

  DISALLOW_COPY_AND_ASSIGN(BackupImpl);
};

// Backup factory to help ICE initialize the objects.
class BackupFactory : public Ice::ObjectFactory {
 public:
  BackupFactory() {}
  virtual Ice::ObjectPtr create(const std::string& type) {
    CHECK_EQ(backup_proto::Backup::ice_staticId(), type);
    return new BackupImpl;
  }
  virtual void destroy() {}

  static void Init(Ice::CommunicatorPtr ic) {
    ic->addObjectFactory(new BackupFactory,
                         backup_proto::Backup::ice_staticId());
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(BackupFactory);
};

}  // namespace backup
#endif  // BACKUP_BACKEND_BTRFS_SERVER_BACKUP_IMPL_H_
