file(GLOB HEADERS_CPPAUDIO "../include/cppaudio/*.h")
source_group("Header Files\\cppaudio" FILES ${HEADERS_CPPAUDIO})

set(SOURCES
  ${HEADERS_CPPAUDIO}
  engine.cpp
  wave.cpp
  reader.cpp
)

add_library(cppaudio ${SOURCES})
target_include_directories(cppaudio PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
