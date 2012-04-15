# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project (backup)

# Allow source files to include things in the Google style, e.g. referenced
# from the project root.
include_directories(AFTER SYSTEM .)

# Add the meta directory to the CMAKE module path so we can pull in custom
# meta files.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/meta)
set(BACKUP_ROOT_PATH ${PROJECT_SOURCE_DIR})
set(SLICE_PROJECT_ARGS --header-ext proto.h --source-ext proto.cc)

##########################################
add_subdirectory(backend)
add_subdirectory(client)
