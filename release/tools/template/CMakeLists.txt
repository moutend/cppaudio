add_executable(__TEST_NAME__ main_test.cpp)
target_link_libraries(__TEST_NAME__ cppaudio)
add_test(
  NAME __TEST_NAME__
  COMMAND $<TARGET_FILE:TestLauncherEngine1to2>
)
