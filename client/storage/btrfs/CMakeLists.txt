cmake_minimum_required (VERSION 2.8)
project(backup_client_storage_btrfs)

set(btrfs_source_files
	 btrfs_storage_backend.cc
	 btrfs_storage_backend.h
)

add_library(backup_client_storage_btrfs ${btrfs_source_files})
