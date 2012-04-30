# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs_proto)

INCLUDE(LintRules)
INCLUDE(Slice2Cpp)

FIND_PACKAGE(Ice REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# The Status object library
# LIBRARY: status_proto
    GENERATE_SLICE2CPP_RULES(
        status_proto_sources status_proto_headers
            status.ice)
    LINT_SOURCES(
        backend.btrfs.proto.status_proto_SOURCES
            status_impl.cc
            status_impl.h)
    ADD_LIBRARY(
        backend.btrfs.proto.status_proto
            ${backend.btrfs.proto.status_proto_SOURCES}
            ${status_proto_sources}
            ${status_proto_headers}
            ${GLOG_LIBRARY})

# The service library
# LIBRARY: service_proto
    GENERATE_SLICE2CPP_RULES(
        service_proto_sources service_proto_headers
            btrfs_backend_service.ice)
    ADD_LIBRARY(
        backend.btrfs.proto.service_proto
            ${service_proto_sources}
            ${service_proto_headers})
    TARGET_LINK_LIBRARIES(
        backend.btrfs.proto.service_proto
            backend.btrfs.proto.backup_descriptor_proto
            backend.btrfs.proto.status_proto
            Ice)
    ADD_DEPENDENCIES(
        backend.btrfs.proto.service_proto
            backend.btrfs.proto.backup_descriptor_proto
            backend.btrfs.proto.status_proto)

# Proto describing the various backup descriptors.
# LIBRARY: backup_descriptor_proto
    GENERATE_SLICE2CPP_RULES(
        backup_descriptor_sources backup_descriptor_headers
            backup_descriptor.ice)
    ADD_LIBRARY(
        backend.btrfs.proto.backup_descriptor_proto
            ${backup_descriptor_sources}
            ${backup_descriptor_headers})
    ADD_DEPENDENCIES(
        backend.btrfs.proto.backup_descriptor_proto
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.proto.backup_descriptor_proto
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.status_proto
            Ice)

# Proto describing backup sets.
# LIBRARY: backup_set_proto
    GENERATE_SLICE2CPP_RULES(
        backup_set_sources backup_set_headers
            backup_set.ice)
    ADD_LIBRARY(
        backend.btrfs.proto.backup_set_proto
            ${backup_set_sources}
            ${backup_set_headers})
    ADD_DEPENDENCIES(
        backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.backup_proto
            backend.btrfs.proto.status_proto
            Ice)

# Proto describing backups.
# LIBRARY: backup_proto
    GENERATE_SLICE2CPP_RULES(
        backup_sources backup_headers
            backup.ice)
    ADD_LIBRARY(
        backend.btrfs.proto.backup_proto
            ${backup_sources}
            ${backup_headers})
    ADD_DEPENDENCIES(
        backend.btrfs.proto.backup_proto
            backend.btrfs.proto.status_proto)
    TARGET_LINK_LIBRARIES(
        backend.btrfs.proto.backup_set_proto
            backend.btrfs.proto.status_proto
            Ice)

