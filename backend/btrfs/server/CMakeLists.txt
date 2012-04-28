# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs_server)

INCLUDE(Slice2Cpp)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Ice REQUIRED)
FIND_PACKAGE(GFlags REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

############################################
# Proto Libraries
############################################

# The service library
# LIBRARY: service_proto
    GENERATE_SLICE2CPP_RULES(
        service_proto_sources service_proto_headers
            btrfs_backend_service.ice)
    ADD_LIBRARY(
        backend.btrfs.server.service_proto
            ${service_proto_sources}
            ${service_proto_headers})
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.service_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_descriptor_proto
            Ice)
    ADD_DEPENDENCIES(
        backend.btrfs.server.service_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_descriptor_proto)

# Proto describing the various backup descriptors.
# LIBRARY: backup_descriptor_proto
    GENERATE_SLICE2CPP_RULES(
        backup_descriptor_sources backup_descriptor_headers
            backup_descriptor.ice)
    ADD_LIBRARY(
        backend.btrfs.server.backup_descriptor_proto
            ${backup_descriptor_sources}
            ${backup_descriptor_headers})
    ADD_DEPENDENCIES(
        backend.btrfs.server.backup_descriptor_proto
            backend.btrfs.server.backup_proto
            backend.btrfs.server.backup_set_proto
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup_descriptor_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_proto
            backend.btrfs.server.backup_set_proto
            Ice)

# Proto describing backup sets.
# LIBRARY: backup_set_proto
    GENERATE_SLICE2CPP_RULES(
        backup_set_sources backup_set_headers
            backup_set.ice)
    ADD_LIBRARY(
        backend.btrfs.server.backup_set_proto
            ${backup_set_sources}
            ${backup_set_headers})
    ADD_DEPENDENCIES(
        backend.btrfs.server.backup_set_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup_set_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_proto
            Ice)

# Proto describing backups.
# LIBRARY: backup_proto
    GENERATE_SLICE2CPP_RULES(
        backup_sources backup_headers
            backup.ice)
    ADD_LIBRARY(
        backend.btrfs.server.backup_proto
            ${backup_sources}
            ${backup_headers})
    ADD_DEPENDENCIES(
        backend.btrfs.server.backup_proto
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup_set_proto
            backend.btrfs.proto.status_proto
            Ice)

############################################
# Server Implementation Targer
############################################

# Implementation of the server-side backup.
# LIBRARY: backup
    ADD_LIBRARY(
        backend.btrfs.server.backup
            backup_impl.cc
            backup_impl.h
            ${Boost_FILESYSTEM_LIBRARY}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.server.backup
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_proto
            Ice)

# Implementation of the server-side backup set.
# LIBRARY: backup_set
    ADD_LIBRARY(
        backend.btrfs.server.backup_set
            backup_set_impl.cc
            backup_set_impl.h
            ${Boost_FILESYSTEM_LIBRARY}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.server.backup_set
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_set_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup_set
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup_set_proto
            Ice)

# The server binary.  This should be run on whatever computers are intended
# to act as servers.
# BINARY: btrfs_backend
    ADD_EXECUTABLE(
        btrfs_backend
            btrfs_backend_main.cc
            btrfs_backend_service_impl.cc
            btrfs_backend_service_impl.h)
    ADD_DEPENDENCIES(
        btrfs_backend
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup
            backend.btrfs.server.backup_descriptor_proto
            backend.btrfs.server.backup_proto
            backend.btrfs.server.backup_set
            backend.btrfs.server.backup_set_proto
            backend.btrfs.server.service_proto)
    TARGET_LINK_LIBRARIES(
        btrfs_backend
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup
            backend.btrfs.server.backup_descriptor_proto
            backend.btrfs.server.backup_proto
            backend.btrfs.server.backup_set
            backend.btrfs.server.backup_set_proto
            backend.btrfs.server.service_proto
            Ice
            IceUtil
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})

