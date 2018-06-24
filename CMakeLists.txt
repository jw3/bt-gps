cmake_minimum_required(VERSION 3.0)
project(tracker)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(BUILD_DIR ${CMAKE_BINARY_DIR})
set(GCC_ARM_PATH /usr/local/gcc-arm/bin/)
set(FIRMWARE_DIR /usr/local/src/particle/firmware)

if (NOT PLATFORM)
    set(PLATFORM electron)
    message(STATUS "defaulting to ${PLATFORM} platform, override with -DPLATFORM")
elseif (NOT PLATFORM MATCHES "^photon$|^electron$")
    message(FATAL_ERROR "invalid platform [${PLATFORM}] (choose 'photon' or 'electron')")
endif ()

message("\n=============================== Configuring firmware for the -=-= ${PLATFORM} =-=- ===============================\n")
configure_file(common/flash.in ${CMAKE_BINARY_DIR}/flash)

include(${PLATFORM})
include(particle)
include(TinyGpsPlus)

add_particle_remote_app(bt-serial TinyGpsPlus)
add_particle_remote_app(asset-tracker TinyGpsPlus)
