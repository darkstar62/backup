# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_client_ui_cli)

INCLUDE(LintRules)

FIND_PACKAGE(GFlags REQUIRED)
FIND_PACKAGE(Glog REQUIRED)
FIND_PACKAGE(Ice REQUIRED)

# BINARY: cli_client
    LINT_SOURCES(
        cli_client_SOURCES
            cli_client.cc
            cli_client.h
            cli_client_main.cc)
    ADD_EXECUTABLE(
        cli_client
            ${cli_client_SOURCES})
    ADD_DEPENDENCIES(
        cli_client
            backend.btrfs.client.storage_backend
            backend.public)
    TARGET_LINK_LIBRARIES(
        cli_client
            backend.btrfs.client.storage_backend
            backend.public
            ${Boost_SYSTEM_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GFLAGS_LIBRARY}
            ${GLOG_LIBRARY}
            Ice)
