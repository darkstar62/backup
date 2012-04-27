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
# LIBRARY: service_proto
    GENERATE_SLICE2CPP_RULES(
        service_proto_sources service_proto_headers
            btrfs_backend_service.ice)
    ADD_LIBRARY(
        backend.btrfs.service_proto
            ${service_proto_sources}
            ${service_proto_headers}
            ${status_proto_headers})
    TARGET_LINK_LIBRARIES(
        backend.btrfs.service_proto
            backend.btrfs.backup_descriptor_proto
            backend.btrfs.status_proto
            Ice)
    ADD_DEPENDENCIES(
        backend.btrfs.service_proto
            backend.btrfs.backup_descriptor_proto
            backend.btrfs.status_proto)

# Proto describing the various backup descriptors.
# LIBRARY: backup_descriptor_proto
    GENERATE_SLICE2CPP_RULES(
        backup_descriptor_sources backup_descriptor_headers
            backup_descriptor.ice)
    ADD_LIBRARY(
        backend.btrfs.backup_descriptor_proto
            ${backup_descriptor_sources}
            ${backup_descriptor_headers}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.backup_descriptor_proto
            backend.btrfs.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.backup_descriptor_proto
            backend.btrfs.status_proto
            Ice)

# The Status object library
# LIBRARY: status_proto
    GENERATE_SLICE2CPP_RULES(
        status_proto_sources status_proto_headers
            status.ice)
    ADD_LIBRARY(
        backend.btrfs.status_proto
            status_impl.cc
            status_impl.h
            ${status_proto_sources}
            ${status_proto_headers}
            ${GLOG_LIBRARY})

# The backend client interface
# LIBRARY: storage_backend
    ADD_LIBRARY(
        backend.btrfs.storage_backend
            btrfs_backup_set.cc
            btrfs_backup_set.h
            btrfs_storage_backend.cc
            btrfs_storage_backend.h
            ${service_proto_headers}
            ${status_proto_headers})
    TARGET_LINK_LIBRARIES(
        backend.btrfs.storage_backend
            backend.btrfs.backup_descriptor_proto
            backend.btrfs.service_proto
            backend.btrfs.status_proto
            boost_thread
            ${Boost_FILESYSTEM_LIBRARY}
            Ice
            IceUtil
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.storage_backend
            backend.btrfs.backup_descriptor_proto
            backend.btrfs.service_proto
            backend.btrfs.status_proto)

# BINARY: btrfs_backend
    ADD_EXECUTABLE(
        btrfs_backend
            btrfs_backend_main.cc
            btrfs_backend_service_impl.cc
            btrfs_backend_service_impl.h)
    ADD_DEPENDENCIES(
        btrfs_backend
            backend.btrfs.backup_descriptor_proto
            backend.btrfs.service_proto
            backend.btrfs.status_proto
            backend.btrfs.storage_backend)
    TARGET_LINK_LIBRARIES(
        btrfs_backend
            backend.btrfs.backup_descriptor_proto
            backend.btrfs.service_proto
            backend.btrfs.status_proto
            backend.btrfs.storage_backend
            Ice
            IceUtil
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})

