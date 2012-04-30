# Copyright (C) 2012, All Rights Reserved.
# Author: Cory Maccarrone <darkstar6262@gmail.com>

# This file contains a macro for passing source files through cpplint.py.
# Any errors found in the linted files prevent the configuration from
# succeeding, requiring that they be addressed before a build can continue.
# Thus, if users are seeing these errors, they're bugs and should be
# reported.
#
# Usage is like this:
#
#   INCLUDE(LintRules)
#
#   LINT_SOURCES(<sources_list> <source_file> [<source_file>] ...)
#   ADD_BINARY(target ${sources_list})
#
# That is, the first argument to LINT_SOURCES is the variable to place linted
# sources into.  This variable will contain a list of all source files that
# passed linting.  Second and subsequent arguments are the source files to
# lint.
#
# Then, you add ${sources_list}, the variable containing the linted source
# files, to your ADD_LIBRARY or ADD_BINARY calls to compile them.
macro( LINT_SOURCES sources_list )
    set( command ${BACKUP_ROOT_PATH}/meta/cpplint.py )
    set( overall_result pass )
    foreach( source_basename ${ARGN} )
        message(STATUS "Linting ${source_basename}")
        set( source_file "${PROJECT_SOURCE_DIR}/${source_basename}" )
        execute_process( COMMAND ${command} ${source_file}
                         RESULT_VARIABLE result )
        if( NOT result EQUAL "0" )
            set( overall_result fail )
        else()
            list( APPEND ${sources_list} ${source_basename} )
        endif()
    endforeach( source_basename )

    if( overall_result STREQUAL fail )
        message( FATAL_ERROR "Lint warnings generated, please address. "
                 "If not a developer, please file a bug." )
    endif()

    set( sources_list ${ARGN} )
endmacro( LINT_SOURCES sources_list )
