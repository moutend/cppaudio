#include <cmath>
#include <cppaudio/reader.h>
#include <cstring>
#include <iostream>

namespace PCMAudio {
WaveReader::WaveReader(Wave *&wave)
    : mWave(wave), mTargetChannels(0), mTargetSamplesPerSec(0),
      mSourceChannels(0), mSourceSamplesPerSec(0), mSourceBytesPerSample(0),
      mSourceTotalBytes(0), mSourceTotalSamples(0), mChannel(0), mDiff(0.0),
      mDiffSum(0.0), mVolume(1.0), mVolumeFactor(0.0), mPause(false) {
  if (wave == nullptr) {
    return;
  }

  mWave = wave;
  mSourceChannels = mWave->Channels();
  mSourceSamplesPerSec = mWave->SamplesPerSec();
  mSourceBytesPerSample = mWave->BitsPerSample() / 8;
  mSourceTotalBytes = static_cast<int32_t>(mWave->DataLength());
  mSourceTotalSamples = mSourceTotalBytes / mSourceBytesPerSample;
}

WaveReader::~WaveReader() {}

void WaveReader::SetFormat(int16_t channels, int32_t samplesPerSec) {
  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;
  mDiff = static_cast<double>(mSourceSamplesPerSec) /
          static_cast<double>(mTargetSamplesPerSec);
}

void WaveReader::Pause() { mVolumeFactor = -0.1; }

void WaveReader::Restart() {
  mPause = false;
  mVolumeFactor = 0.1;
}

bool WaveReader::IsDone() {
  return static_cast<int32_t>(floor(mDiffSum)) * mSourceChannels >
         mSourceTotalSamples - 1;
}

void WaveReader::Next() {
  if (mPause || IsDone()) {
    return;
  }

  mChannel = (mChannel + 1) % mTargetChannels;

  if (mChannel == 0) {
    mDiffSum += mDiff;
    mVolume = mVolume + mVolumeFactor;
  }
  if (mVolume > 1.0) {
    mVolume = 1.0;
    mVolumeFactor = 0.0;
  }
  if (mVolume < 0.0) {
    mVolume = 0.0;
    mVolumeFactor = 0.0;

    mPause = true;
  }
}

int32_t WaveReader::Read() {
  double ratio = mDiffSum - floor(mDiffSum);
  int32_t base = static_cast<int32_t>(floor(mDiffSum)) * mSourceChannels;
  int32_t channel = mChannel % mSourceChannels;
  int32_t index1 =
      mSourceBytesPerSample * channel + mSourceBytesPerSample * base;
  int32_t index2 = mSourceBytesPerSample * channel +
                   mSourceBytesPerSample * (base + mSourceChannels);
  int32_t value1 = index1 < mSourceTotalBytes ? ReadInt32t(index1) : 0;
  int32_t value2 = index2 < mSourceTotalBytes ? ReadInt32t(index2) : 0;
  double result = mVolume * (value1 * (1.0 - ratio) + value2 * ratio);

  return static_cast<int32_t>(result);
}

/*
 * ReadInt32T returns a sample as 32 bit integer.
 *
 * 16 bit value 0x1234 will converted to 0x12340000.
 * 24 bit value 0x123456 will converted to 0x12345600.
 * 32 bit value 0x12345678 stays the same.
 *
 * Note: index is based on bytes, not samples.
 */
int32_t WaveReader::ReadInt32t(int32_t index) {
  int32_t s32{};

  std::memcpy(&s32, mWave->Data() + index, mSourceBytesPerSample);

  return s32 << (8 * (4 - mSourceBytesPerSample));
}

SilentReader::SilentReader(double duration /* ms */)
    : mTargetChannels(0), mTargetSamplesPerSec(0), mTargetTotalSamples(0),
      mDiff(1.0), mDiffSum(0.0), mDuration(duration), mPause(false) {}

SilentReader::~SilentReader() {}

void SilentReader::SetFormat(int16_t channels, int32_t samplesPerSec) {
  mDuration = mDuration - static_cast<double>(floor(mDiffSum)) /
                              static_cast<double>(samplesPerSec) * 1000.0;

  if (mDuration < 0.0) {
    mDuration = 0.0;
  }

  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;
  mTargetTotalSamples = mTargetChannels * mTargetSamplesPerSec *
                        static_cast<int32_t>(mDuration / 1000.0);
}

void SilentReader::Restart() { mPause = false; }

void SilentReader::Pause() { mPause = true; }

bool SilentReader::IsDone() {
  return mDuration < 0.0 ||
         static_cast<int32_t>(floor(mDiffSum)) * mTargetChannels >
             mTargetTotalSamples - 1;
}

void SilentReader::Next() {
  if (mPause || IsDone()) {
    return;
  }

  mChannel = (mChannel + 1) % mTargetChannels;

  if (mChannel == 0) {
    mDiffSum += mDiff;
  }
}

int32_t SilentReader::Read() { return 0; }
} // namespace PCMAudio
