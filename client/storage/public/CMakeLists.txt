cmake_minimum_required (VERSION 2.8)
project(backup_client_storage_public)

set(public_source_files
	 backup.h
	 backup_set.h
	 file_list.h
)
add_library(backup_client_storage_public ${public_source_files})

