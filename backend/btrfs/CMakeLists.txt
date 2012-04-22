# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs)

INCLUDE(Slice2Cpp)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Ice REQUIRED)
FIND_PACKAGE(GFlags REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# The service library
# LIBRARY: btrfs_backend_service_lib
    GENERATE_SLICE2CPP_RULES(
        proto_sources proto_headers
            btrfs_backend_service.ice)
    ADD_LIBRARY(
        btrfs_backend_service_lib
            ${proto_sources}
            ${proto_headers})

# The backend client interface
# LIBRARY: backup_client_storage_btrfs
    ADD_LIBRARY(
        backup_client_storage_btrfs
            btrfs_backup_set.h
            btrfs_storage_backend.cc
            btrfs_storage_backend.h)
    TARGET_LINK_LIBRARIES(
        backup_client_storage_btrfs
            btrfs_backend_service_lib
            boost_thread
            Ice
            IceUtil
            ${GLOG_LIBRARY}
            ${PROTOBUF_LIBRARY})
    ADD_DEPENDENCIES(
        backup_client_storage_btrfs
            btrfs_backend_service_lib)

# The backend server process
# LIBRARY: btrfs_backend_lib
    ADD_LIBRARY(
        btrfs_backend_lib
            btrfs_backend_service_impl.cc
            btrfs_backend_service_impl.h)
    TARGET_LINK_LIBRARIES(
        btrfs_backend_lib
            btrfs_backend_service_lib
            Ice
            IceUtil
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        btrfs_backend_lib
            btrfs_backend_service_lib)

# BINARY: btrfs_backend
    ADD_EXECUTABLE(
        btrfs_backend
            btrfs_backend_main.cc)
    ADD_DEPENDENCIES(
        btrfs_backend
            btrfs_backend_lib)
    TARGET_LINK_LIBRARIES(
        btrfs_backend
            btrfs_backend_lib)

