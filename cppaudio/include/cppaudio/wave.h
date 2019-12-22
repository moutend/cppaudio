#pragma once

#include <cstdint>
#include <iostream>
#include <string>

namespace PCMAudio {
class Wave {
public:
  Wave(const char *buffer, size_t bufferLength);
  Wave(std::istream &input);
  ~Wave();

  int16_t FormatTag();
  int16_t Channels();
  int32_t SamplesPerSec();
  int32_t AvgBytesPerSec();
  int16_t BlockAlign();
  int16_t BitsPerSample();

  std::streamsize DataLength();
  char *Data();
  bool HasLoaded();

private:
  void load(std::istream &input);

  int16_t mFormatTag;
  int16_t mChannels;
  int32_t mSamplesPerSec;
  int32_t mAvgBytesPerSec;
  int16_t mBlockAlign;
  int16_t mBitsPerSample;

  std::streamsize mDataLength;
  char *mData = nullptr;
  bool mLoaded = false;
};
} // namespace PCMAudio
