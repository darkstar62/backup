# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project(backup_backend_btrfs)

include(Slice2Cpp)

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Ice REQUIRED)
find_package(GFlags REQUIRED)
find_package(Glog REQUIRED)

# The service library
generate_slice2cpp_rules(proto_sources proto_headers
    btrfs_backend_service.ice)
add_library(btrfs_backend_service_lib
    ${proto_sources} ${proto_headers})

# The backend client interface
add_library(backup_client_storage_btrfs
    btrfs_backup_set.h
    btrfs_storage_backend.cc
    btrfs_storage_backend.h
)
target_link_libraries(backup_client_storage_btrfs
    btrfs_backend_service_lib
    boost_thread
    Ice
    IceUtil
    ${GLOG_LIBRARY}
    ${PROTOBUF_LIBRARY})
add_dependencies(backup_client_storage_btrfs btrfs_backend_service_lib)

# The backend server process
add_library(btrfs_backend_lib
    btrfs_backend_service_impl.cc
    btrfs_backend_service_impl.h)
target_link_libraries(btrfs_backend_lib
    btrfs_backend_service_lib
    Ice
    IceUtil
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${GFLAGS_LIBRARY}
    ${GLOG_LIBRARY})
add_dependencies(btrfs_backend_lib
    btrfs_backend_service_lib)

# The binary
add_executable(btrfs_backend btrfs_backend_main.cc)
add_dependencies(btrfs_backend btrfs_backend_lib)
target_link_libraries(btrfs_backend btrfs_backend_lib)

