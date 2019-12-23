add_executable(launcher_engine_test main_test.cpp)
target_link_libraries(launcher_engine_test cppaudio)
add_test(
  NAME launcher_engine_test
  COMMAND $<TARGET_FILE:launcher_engine_test>
)
