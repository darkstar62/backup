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
        service_proto_sources service_proto_headers
            btrfs_backend_service.ice)
    ADD_LIBRARY(
        btrfs_backend_service_lib
            ${service_proto_sources}
            ${service_proto_headers}
            ${status_proto_headers})
    TARGET_LINK_LIBRARIES(
        btrfs_backend_service_lib
            btrfs_backend_status_lib
            Ice)
    ADD_DEPENDENCIES(
        btrfs_backend_service_lib
            btrfs_backend_status_lib)

# The Status object library
# LIBRARY: btrfs_backend_status_lib
    GENERATE_SLICE2CPP_RULES(
        status_proto_sources status_proto_headers
            status.ice)
    ADD_LIBRARY(
        btrfs_backend_status_lib
            status_impl.cc
            status_impl.h
            ${status_proto_sources}
            ${status_proto_headers}
            ${GLOG_LIBRARY})

# The backend client interface
# LIBRARY: backup_client_storage_btrfs
    ADD_LIBRARY(
        backup_client_storage_btrfs
            btrfs_backup_set.cc
            btrfs_backup_set.h
            btrfs_storage_backend.cc
            btrfs_storage_backend.h
            ${service_proto_headers}
            ${status_proto_headers})
    TARGET_LINK_LIBRARIES(
        backup_client_storage_btrfs
            btrfs_backend_service_lib
            btrfs_backend_status_lib
            boost_thread
            Ice
            IceUtil
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backup_client_storage_btrfs
            btrfs_backend_service_lib
            btrfs_backend_status_lib)

# The backend server process
# LIBRARY: btrfs_backend_lib
    ADD_LIBRARY(
        btrfs_backend_lib
            btrfs_backend_service_impl.cc
            btrfs_backend_service_impl.h)
    TARGET_LINK_LIBRARIES(
        btrfs_backend_lib
            btrfs_backend_service_lib
            btrfs_backend_status_lib
            Ice
            IceUtil
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        btrfs_backend_lib
            btrfs_backend_service_lib
            btrfs_backend_status_lib)

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

