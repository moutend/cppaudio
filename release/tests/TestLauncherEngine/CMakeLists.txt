add_executable(TestLauncherEngine main_test.cpp)
target_link_libraries(TestLauncherEngine cppaudio)
add_test(
  NAME TestLauncherEngine
  COMMAND $<TARGET_FILE:TestLauncherEngine>
)
