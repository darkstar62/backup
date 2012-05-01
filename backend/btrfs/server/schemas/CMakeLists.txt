# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs_server_schemas)

INCLUDE(LintRules)

FIND_PACKAGE(Boost COMPONENTS system REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# Schema library.  This includes all schema versions.
# LIBRARY: schema
    LINT_SOURCES(
        backend.btrfs.server.schemas.schema_SOURCES
            schema.cc
            schema.h
            schema_version_1.cc
            schema_version_1.h)
    ADD_LIBRARY(
        backend.btrfs.server.schemas.schema
            ${backend.btrfs.server.schemas.schema_SOURCES})
    ADD_DEPENDENCIES(
        backend.btrfs.server.schemas.schema
            backend.btrfs.proto.status_proto
            backend.btrfs.server.sqlite)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.server.schemas.schema
            backend.btrfs.proto.status_proto
            backend.btrfs.server.sqlite
            ${Boost_SYSTEM_LIBRARY}
            ${SQLITE3_LIBRARY}
            ${GLOG_LIBRARY})

