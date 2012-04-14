# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project(backup_client_storage_btrfs)

# Requires protocol buffers and casock
find_package(Boost REQUIRED)
find_package(Casock REQUIRED)
find_package(Glog REQUIRED)
find_package(ProtocolBuffers REQUIRED)

set(btrfs_source_files
    btrfs_storage_backend.cc
    btrfs_storage_backend.h
)
add_library(backup_client_storage_btrfs ${btrfs_source_files})
add_dependencies(backup_client_storage_btrfs
                 btrfs_backend_service_proto_lib)
target_link_libraries(backup_client_storage_btrfs
    btrfs_backend_service_proto_lib
    ${CASOCK_LIBRARY_RPC_PB_BASE}
    ${CASOCK_LIBRARY_RPC_PB_CLIENT}
    ${CASOCK_LIBRARY_RPC_SIO_PB_BASE}
    ${CASOCK_LIBRARY_RPC_SIO_PB_CLIENT}
    ${CASOCK_LIBRARY_SIOBASE}
    ${CASOCK_LIBRARY_SIOCLIENT}
    ${CASOCK_LIBRARY_UTIL}
    ${GLOG_LIBRARY}
    ${PROTOBUF_LIBRARY})
