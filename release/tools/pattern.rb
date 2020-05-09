#!/usr/bin/env ruby

puts "template_name,source_samples_per_sec,source_channels,source_bits_per_sample,target_samples_per_sec,target_channels,target_bits_per_sample"
[
  # TestKickPattern01",
  # TestKickPattern02",
  # TestKickPattern03",
  # TestKickPattern04",
  # TestKickPattern05",
  # TestKickPattern06",
  # TestKickPattern07",
  # TestKickPattern08",
  # TestKickPattern09",
  # TestKickPattern10",
  # TestKickPattern11",
  # TestKickPattern12",
  # TestKickPattern13",
  # TestKickPattern14",
  # TestKickPattern15",
  # TestKickPattern16",
  # TestKickPattern17",
  # TestKickPattern18",
  # TestKickPattern19",
  # TestKickPattern20",
  # TestKickPattern21",
  # TestKickPattern22",
  # TestKickPattern23",
  # TestKickPattern24",
  # TestKickPattern25",
  # TestKickPattern26",
  # TestKickPattern27",
  # TestKickPattern28",
  # TestKickPattern29",
  # TestKickPattern30",
  # TestKickPattern31",
  # TestKickPattern32",
  # TestKickPattern33",
  # TestKickPattern34",
  # TestKickPattern35",
  # TestWavePattern01",
  # TestWavePattern02",
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
