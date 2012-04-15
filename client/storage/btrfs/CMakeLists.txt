# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project(backup_client_storage_btrfs)

# Required packages
find_package(Boost REQUIRED)
find_package(Glog REQUIRED)
find_package(Ice REQUIRED)
find_package(ProtocolBuffers REQUIRED)

set(btrfs_source_files
    btrfs_backup_set.h
    btrfs_storage_backend.cc
    btrfs_storage_backend.h
)
add_library(backup_client_storage_btrfs ${btrfs_source_files})
target_link_libraries(backup_client_storage_btrfs
    btrfs_backend_service_lib
    boost_thread
    Ice
    IceUtil
    ${GLOG_LIBRARY}
    ${PROTOBUF_LIBRARY})
add_dependencies(backup_client_storage_btrfs btrfs_backend_service_lib)
