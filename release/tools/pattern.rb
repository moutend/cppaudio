#!/usr/bin/env ruby

puts "template_name,source_samples_per_sec,source_channels,source_bits_per_sample,target_samples_per_sec,target_channels,target_bits_per_sample"

[
  "TestLauncherEnginePattern1",
  "TestLauncherEnginePattern2",
  "TestLauncherEnginePattern3",
  "TestLauncherEnginePattern4",
  "TestLauncherEnginePattern5",
  "TestLauncherEnginePattern6",
  "TestLauncherEnginePattern7",
  "TestLauncherEnginePattern8",
  "TestLauncherEnginePattern9",
  "TestLauncherEnginePattern10",
  "TestLauncherEnginePattern11",
  "TestLauncherEnginePattern12",
  "TestLauncherEnginePattern13",
  "TestLauncherEnginePattern14",
  "TestLauncherEnginePattern15",
  "TestLauncherEnginePattern16",
  "TestRingEnginePattern1",
  "TestRingEnginePattern2",
  "TestRingEnginePattern3",
  "TestRingEnginePattern4",
  "TestRingEnginePattern5",
  "TestRingEnginePattern6",
  "TestRingEnginePattern7",
  "TestRingEnginePattern8",
  "TestRingEnginePattern9",
  "TestRingEnginePattern10",
  "TestRingEnginePattern11",
  "TestRingEnginePattern12",
  "TestRingEnginePattern13",
  "TestRingEnginePattern14",
  "TestRingEnginePattern15",
  "TestRingEnginePattern16",
].each do |template_name|
  [44100, 48000, 96000].each do |source_samples_per_sec|
    [1,2].each do |source_channels|
      [16, 24, 32].each do |source_bits_per_sample|
        [44100, 48000, 96000].each do |target_samples_per_sec|
          [2, 6].each do |target_channels|
            [32].each do |target_bits_per_sample|
              puts "#{template_name},#{source_samples_per_sec},#{source_channels},#{source_bits_per_sample},#{target_samples_per_sec},#{target_channels},#{target_bits_per_sample}"
            end
          end
        end
      end
    end
  end
end
