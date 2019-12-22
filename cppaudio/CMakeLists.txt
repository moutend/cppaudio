cmake_minimum_required(VERSION 3.12)
project(cppaudio CXX)
enable_testing()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR OR EXISTS "${CMAKE_BINARY_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

set(CMAKE_CXX_STANDARD 17)

set(CPPAUDIO_VERSION_MAJOR 0)
set(CPPAUDIO_VERSION_MINOR 1)
set(CPPAUDIO_VERSION_REVISION 0)

add_subdirectory(src)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
