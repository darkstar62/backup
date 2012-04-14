# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
cmake_minimum_required (VERSION 2.8)
project(backup_client_ui_cli)

#####################
# cli_client
#####################

set(cli_client_sources cli_client.cc)
set(cli_client_deps backup_client_storage_btrfs)
set(cli_client_libraries
    backup_client_storage_btrfs
    ${GLOG_LIBRARY})

add_executable(cli_client ${cli_client_sources})
add_dependencies(cli_client ${cli_client_deps})
target_link_libraries(cli_client ${cli_client_libraries})

#####################
