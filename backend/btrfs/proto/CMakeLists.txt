# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_backend_btrfs_proto)

INCLUDE(Slice2Cpp)

FIND_PACKAGE(Ice REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# The Status object library
# LIBRARY: status_proto
    GENERATE_SLICE2CPP_RULES(
        status_proto_sources status_proto_headers
            status.ice)
    ADD_LIBRARY(
        backend.btrfs.proto.status_proto
            status_impl.cc
            status_impl.h
            ${status_proto_sources}
            ${status_proto_headers}
            ${GLOG_LIBRARY})

