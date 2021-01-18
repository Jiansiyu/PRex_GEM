#----------------------------------------------------------------------------
# Setup the project
#

## Set the default build type
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set build type (Debug is default)")

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(replayer)

## Disable in-source builds!
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "cmake cannot build PREX-counting in the root directory: "
    "${PROJECT_SOURCE_DIR}\nsince it conflicts with our old Makefile."
    " Create a build directory anywhere, change to it and re-run cmake."
    )
endif()


#8#----------------------------------------------------------------------------
## Set the sources which have a corresponding .h file here
set(sources ./replay.C )
## Make corresponding header files
string(REGEX REPLACE .cxx .h headers "${sources}")
## Append a few extra header files in the repository.  (But do not append the
## sbs_LinkDef.h header here, that is further down this cmake config.
#list(APPEND headers "DebugDef.h")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

## Setup default variables (copied from Makefile)
#FIXME: should be configure'd:
set(EXTRAWARN ON CACHE BOOL "g++ Wextra warn options if supported")

##----------------------------------------------------------------------------
set(VERBOSE ON CACHE BOOL "Compile extra code for printing verbose messages")
set(TESTCODE ON CACHE BOOL "Compile extra diagnostic code (extra computations and global variables")
set(MCDATA ON CACHE BOOL "Compile support code for MC input data")
#set(CXXMAXERRORS 0 CACHE INT "Maximum number of allowed errors before cxx stops")
list(APPEND PREXEXTRADEF_LIST VERBOSE TESTCODE MCDATA)

#----------------------------------------------------------------------------
# Find ROOT (using custom FindROOT.cmake taken from G4SBS)
set(minimum_root_version 5.10)
find_package(ROOT REQUIRED)


#----------------------------------------------------------------------------
## Setup the PODD Analyzer
find_package(Podd 1.6 REQUIRED)


# Required dependencies
find_package(EVIO CONFIG QUIET)
if(NOT EVIO_FOUND)
  find_package(EVIO MODULE)
  if(NOT EVIO_FOUND)
    include(FetchBuildEVIO)
  endif()
endif()


##----------------------------------------------------------------------------
## Check for SStream support
include(TestForSSTREAM)
if(CMAKE_HAS_ANSI_STRING_STREAM)
  set(HAS_SSTREAM ON)
  list(APPEND PREXEXTRADEF_LIST HAS_SSTREAM)
endif()

##----------------------------------------------------------------------------
## OS Type (not sure what exactly uses this, but since it's in the Makefile
## I'll just copy it here too...
set(PREX_LINKFLAGS "")
if(UNIX) ## Can be anything from Linux to Mac OS X
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
  if(APPLE)
    set(MACVERS ON)
    list(APPEND PREXEXTRADEF_LIST MACVERS)
    set(PREX_LINKFLAGS "${PREX_LINKFLAGS} -Wl,-undefined,dynamic_lookup")
  else()
    set(LINUXVERS ON)
    list(APPEND PREXEXTRADEF_LIST LINUXVERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
    if(EXTRAWARN AND CMAKE_COMPILER_IS_GNUCXX AND NOT
        CMAKE_CXX_COMPILER_VERSION LESS 4)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wno-missing-field-initializers")
    endif()
    if(CXXMAXERRORS GREATER 0)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=${CXXMAXERRORS}")
    endif()
  endif()
endif() ## I guess no Windows support?

include_directories("/home/newdriver/Storage/Research/PRex_GEM/PRex_Replay/PRexAnalyzer")


##----------------------------------------------------------------------------
## Copy over some flags we have set in the Makefile
if(VERBOSE)
  list(APPEND VERBOSE)
endif()

include_directories(${ROOT_INCLUDE_DIR} ${Podd_INCLUDE_DIR} ${EVIO_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

## Generate the ROOT dictionary
set(PROJECT_DICTIONARY ${PROJECT_NAME}Dict)
ROOT_GENERATE_DICTIONARY( ${PROJECT_DICTIONARY} ${headers}
  LINKDEF ${PROJECT_NAME}_LinkDef.h)

## Create the library
add_library(${PROJECT_NAME} SHARED ${sources} ${headers} ${PROJECT_DICTIONARY}.cxx )
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " ${PREX_LINKFLAGS}")
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES})
foreach(_def ${PREXEXTRADEF_LIST})
  if(${${_def}})
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${_def})
  endif()
endforeach()

message(AUTHOR_WARNING ${DEV_WARNING})

