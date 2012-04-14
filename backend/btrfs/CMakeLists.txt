# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project(backup_backend_btrfs)

# We need protocol buffers for this library.
find_package(Boost REQUIRED)
find_package(Casock REQUIRED)
find_package(ProtocolBuffers REQUIRED)

# Build the protobuf library
wrap_proto(btrfs_proto btrfs_backend_service.proto)
add_library(btrfs_backend_service_proto_lib ${btrfs_proto})
target_link_libraries(btrfs_backend_service_proto_lib ${PROTOBUF_LIBRARY})

# The backend
set(backend_sources
    btrfs_backend.cc
    btrfs_backend_service_impl.cc
    btrfs_backend.h
    btrfs_backend_service_impl.h)
add_library(btrfs_backend_lib ${backend_sources})
target_link_libraries(btrfs_backend_lib
    btrfs_backend_service_proto_lib
    ${CASOCK_LIBRARY_RPC_PB_BASE}
    ${CASOCK_LIBRARY_RPC_PB_SERVER}
    ${CASOCK_LIBRARY_RPC_SIO_PB_BASE}
    ${CASOCK_LIBRARY_RPC_SIO_PB_SERVER}
    ${CASOCK_LIBRARY_SIOBASE}
    ${CASOCK_LIBRARY_UTIL}
    ${GLOG_LIBRARY})

# The binary
add_executable(btrfs_backend btrfs_backend_main.cc)
target_link_libraries(btrfs_backend btrfs_backend_lib)
