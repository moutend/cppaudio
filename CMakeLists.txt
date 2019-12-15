cmake_minimum_required(VERSION 3.12)
project(
  cppaudio
  VERSION 0.1.0
  DESCRIPTION "Utility for reading and writing PCM audio data"
  LANGUAGES CXX
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR OR EXISTS "${CMAKE_BINARY_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

if(NOT WIN32)
  message(FATAL "This project targets Windows")
endif()

set(CMAKE_CXX_STANDARD 17)
set(
  LIBRARY_SOURCES
  src/engine.cpp
  src/engine.h
  src/reader.cpp
  src/reader.h
  src/wave.cpp
  src/wave.h
)
add_library(cppaudio ${LIBRARY_SOURCES})
