# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs_server)

INCLUDE(LintRules)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Ice REQUIRED)
FIND_PACKAGE(GFlags REQUIRED)
FIND_PACKAGE(Glog REQUIRED)
FIND_PACKAGE(Sqlite3 REQUIRED)

# SQLite3 wrapper library.
# LIBRARY: sqlite
    LINT_SOURCES(
        backend.btrfs.server.sqlite_SOURCES
            sqlite.cc
            sqlite.h)
    ADD_LIBRARY(
        backend.btrfs.server.sqlite
            ${backend.btrfs.server.sqlite_SOURCES}
            ${Boost_SYSTEM_LIBRARY}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.server.sqlite
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.sqlite
            backend.btrfs.proto.status_proto
            ${Boost_SYSTEM_LIBRARY}
            ${GLOG_LIBRARY}
            ${SQLITE3_LIBRARY})

# Implementation of the server-side backup.
# LIBRARY: backup
    LINT_SOURCES(
        backend.btrfs.server.backup_SOURCES
            backup_impl.cc
            backup_impl.h)
    ADD_LIBRARY(
        backend.btrfs.server.backup
            ${backend.btrfs.server.backup_SOURCES}
            ${Boost_FILESYSTEM_LIBRARY}
            ${GLOG_LIBRARY})
    ADD_DEPENDENCIES(
        backend.btrfs.server.backup
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.sqlite)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.sqlite
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
            backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.backup_set
            backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.status_proto
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
            backend.btrfs.proto.backup_descriptor_proto
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.service_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup
            backend.btrfs.server.backup_set)
    TARGET_LINK_LIBRARIES(
        btrfs_backend
            backend.btrfs.proto.backup_descriptor_proto
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.service_proto
            backend.btrfs.proto.status_proto
            backend.btrfs.server.backup
            backend.btrfs.server.backup_set
            Ice
            IceUtil
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})

