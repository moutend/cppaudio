cmake_minimum_required(VERSION 3.12)
project(
  cppaudio
  VERSION 0.1.0
  DESCRIPTION "Utility for reading and writing PCM audio data"
  LANGUAGES CXX
)

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
