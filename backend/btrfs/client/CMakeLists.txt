# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs_client)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Ice REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# The backend client interface
# LIBRARY: storage_backend
    ADD_LIBRARY(
        backend.btrfs.client.storage_backend
            btrfs_backup_set.cc
            btrfs_backup_set.h
            btrfs_storage_backend.cc
            btrfs_storage_backend.h
            ${service_proto_headers}
            ${status_proto_headers})
    TARGET_LINK_LIBRARIES(
        backend.btrfs.client.storage_backend
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_descriptor_proto
            backend.btrfs.server.service_proto
            boost_thread
            ${Boost_FILESYSTEM_LIBRARY}
            Ice
            IceUtil
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.client.storage_backend
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_descriptor_proto
            backend.btrfs.server.service_proto)
