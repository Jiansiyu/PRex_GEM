cmake_minimum_required(VERSION 3.12)
project(runVarifier)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#find the package
find_package(ROOT)

find_package(EVIO CONFIG QUIET)
# Required dependencies
find_package(EVIO CONFIG QUIET)
if(NOT EVIO_FOUND)
    find_package(EVIO MODULE)
    if(NOT EVIO_FOUND)
        include(FetchBuildEVIO)
    endif()
endif()

include_directories(${ROOT_INCLUDE_DIR} ${EVIO_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 11)


add_executable(runVerifier main.cpp runVerifier.cpp runVerifier.h)