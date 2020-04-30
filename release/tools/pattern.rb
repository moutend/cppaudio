#!/usr/bin/env ruby

puts "template_name,source_samples_per_sec,source_channels,source_bits_per_sample,target_samples_per_sec,target_channels,target_bits_per_sample"

[
  # "TestNormalLauncherEngine",
  # "TestInterruptLauncherEngine",
  # "TestFadeLauncherEngine",
  # "TestBlankLauncherEngine",
  # "TestStutterLauncherEngine",
  # "TestDoubleFormatLauncherEngine",
  # "TestHalfFormatLauncherEngine",
  # "TestFixedFormatLauncherEngine",
  "TestDelayFormatLauncherEngine",
  # "TestDelayLauncherEngine"
].each do |template_name|
  [44100, 48000, 96000].each do |source_samples_per_sec|
    [1,2].each do |source_channels|
      [16, 24, 32].each do |source_bits_per_sample|
        [44100, 48000, 96000].each do |target_samples_per_sec|
          [2, 6].each do |target_channels|
            [32].each do |target_bits_per_sample|
              puts "\"#{template_name}\",#{source_samples_per_sec},#{source_channels},#{source_bits_per_sample},#{target_samples_per_sec},#{target_channels},#{target_bits_per_sample}"
            end
          end
        end
      end
    end
  end
end
