# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_client_storage_public)

INCLUDE(LintRules)

# LIBRARY: public
    LINT_SOURCES(
        backend.public_SOURCES
            backup.h
            backup_set.h
            file_list.cc
            file_list.h)
    ADD_LIBRARY(
        backend.public
          ${backend.public_SOURCES})

