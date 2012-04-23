# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_client_ui_cli)

FIND_PACKAGE(GFlags REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# Library to handle commands from the CLI client.
# LIBRARY: cli_client_lib
    ADD_LIBRARY(
        cli_client_lib
            cli_client.cc
            cli_client.h)
    TARGET_LINK_LIBRARIES(
        cli_client_lib
            backup_client_storage_btrfs
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            Ice)
    ADD_DEPENDENCIES(
        cli_client_lib
            backup_client_storage_btrfs)

# BINARY: cli_client
    ADD_EXECUTABLE(
        cli_client
            cli_client_main.cc)
    ADD_DEPENDENCIES(
        cli_client
            cli_client_lib)
    TARGET_LINK_LIBRARIES(
        cli_client
            cli_client_lib
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})
