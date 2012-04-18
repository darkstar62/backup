# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project(backup_backend_btrfs)

include(Slice2Cpp)

# We need protocol buffers for this library.
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Ice REQUIRED)
find_package(GFlags REQUIRED)
find_package(Glog REQUIRED)

generate_slice2cpp_rules(proto_sources proto_headers
    btrfs_backend_service.ice)

# The service library
add_library(btrfs_backend_service_lib
    ${proto_sources} ${proto_headers})

# The backend
set(backend_sources
    btrfs_backend_service_impl.cc
    btrfs_backend_service_impl.h)
add_library(btrfs_backend_lib ${backend_sources})
target_link_libraries(btrfs_backend_lib
    btrfs_backend_service_lib
    Ice
    IceUtil
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${GFLAGS_LIBRARY}
    ${GLOG_LIBRARY})
add_dependencies(btrfs_backend_lib btrfs_backend_service_lib)

# The binary
add_executable(btrfs_backend btrfs_backend_main.cc)
add_dependencies(btrfs_backend btrfs_backend_lib)
target_link_libraries(btrfs_backend btrfs_backend_lib)
