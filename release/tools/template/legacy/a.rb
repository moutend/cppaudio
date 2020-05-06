n = 0
[
"TestLauncherEnginePattern1.cpp",
"TestLauncherEnginePattern2.cpp",
"TestLauncherEnginePattern3.cpp",
"TestLauncherEnginePattern4.cpp",
"TestLauncherEnginePattern5.cpp",
"TestLauncherEnginePattern6.cpp",
"TestLauncherEnginePattern7.cpp",
"TestLauncherEnginePattern8.cpp",
"TestLauncherEnginePattern9.cpp",
"TestLauncherEnginePattern10.cpp",
"TestLauncherEnginePattern11.cpp",
"TestLauncherEnginePattern12.cpp",
"TestLauncherEnginePattern13.cpp",
"TestLauncherEnginePattern14.cpp",
"TestLauncherEnginePattern15.cpp",
"TestLauncherEnginePattern16.cpp",
"TestRingEnginePattern1.cpp",
"TestRingEnginePattern2.cpp",
"TestRingEnginePattern3.cpp",
"TestRingEnginePattern4.cpp",
"TestRingEnginePattern5.cpp",
"TestRingEnginePattern6.cpp",
"TestRingEnginePattern7.cpp",
"TestRingEnginePattern8.cpp",
"TestRingEnginePattern9.cpp",
"TestRingEnginePattern10.cpp",
"TestRingEnginePattern11.cpp",
"TestRingEnginePattern12.cpp",
"TestRingEnginePattern13.cpp",
"TestRingEnginePattern14.cpp",
"TestRingEnginePattern15.cpp",
"TestRingEnginePattern16.cpp"
].each do |name|
  n += 1
  dst = "TestPattern#{n.to_s.rjust(3, '0')}.cpp"
  `cp #{name} #{dst}`
end
