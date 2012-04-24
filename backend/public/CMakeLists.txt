# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(backup_client_storage_public)

# LIBRARY: public
    ADD_LIBRARY(
        backend.public
	        backup.h
            backup_set.h
	        file_list.h)

