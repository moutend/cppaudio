add_executable(ring_engine_test main_test.cpp)
target_link_libraries(ring_engine_test cppaudio)
add_test(
  NAME ring_engine_test
  COMMAND $<TARGET_FILE:ring_engine_test>
)
