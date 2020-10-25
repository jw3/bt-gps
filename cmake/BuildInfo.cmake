include(${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include_directories(${CMAKE_BINARY_DIR}/include)
configure_file(${CMAKE_SOURCE_DIR}/common/BuildInfo.h.in ${CMAKE_BINARY_DIR}/include/BuildInfo.h)
