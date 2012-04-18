cmake_minimum_required (VERSION 2.8)
project(backup_client_storage_public)

add_library(backup_backend_public
	 backup.h
	 backup_set.h
	 file_list.h)

