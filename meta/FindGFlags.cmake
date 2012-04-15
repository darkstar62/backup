# Try to find google flags (gflags)
#
# Use as FIND_PACKAGE(GFlags)
#
#  GFLAGS_FOUND - system has the protocol buffers library
#  GFLAGS_INCLUDE_DIR - the zip include directory
#  GFLAGS_LIBRARY - Link this to use the zip library

IF (GFLAGS_LIBRARY AND GFLAGS_INCLUDE_DIR)
  # in cache already
  SET(GFLAGS_FOUND TRUE)
ELSE (GFLAGS_LIBRARY AND GFLAGS_INCLUDE_DIR)

  FIND_PATH(GFLAGS_INCLUDE_DIR gflags/gflags.h
    /usr/include
    /usr/local/include
  )

  FIND_LIBRARY(GFLAGS_LIBRARY NAMES gflags
    PATHS
    ${GNUWIN32_DIR}/lib
  )

  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(protobuf DEFAULT_MSG GFLAGS_INCLUDE_DIR GFLAGS_LIBRARY)

  # ensure that they are cached
  SET(GFLAGS_INCLUDE_DIR ${GFLAGS_INCLUDE_DIR} CACHE INTERNAL "The protocol buffers include path")
  SET(GFLAGS_LIBRARY ${GFLAGS_LIBRARY} CACHE INTERNAL "The libraries needed to use protocol buffers library")

ENDIF (GFLAGS_LIBRARY AND GFLAGS_INCLUDE_DIR)

