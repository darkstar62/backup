# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_client_ui_cli)

FIND_PACKAGE(GFlags REQUIRED)
FIND_PACKAGE(Glog REQUIRED)

# BINARY: cli_client
    ADD_EXECUTABLE(
        cli_client
            cli_client.cc)
    ADD_DEPENDENCIES(
        cli_client
            backup_client_storage_btrfs)
    TARGET_LINK_LIBRARIES(
        cli_client
            backup_client_storage_btrfs
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY})
