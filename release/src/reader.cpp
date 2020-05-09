#include <cmath>
#include <cppaudio/reader.h>
#include <cstring>
#include <iostream>

namespace PCMAudio {
WaveReader::WaveReader(Wave *&wave, double pan)
    : mWave{wave}, mTargetChannels{}, mTargetSamplesPerSec{}, mSourceChannels{},
      mSourceSamplesPerSec{}, mSourceBytesPerSample{}, mSourceTotalBytes{},
      mSourceTotalSamples{}, mChannel{}, mDelayChannel{-1}, mDelaySamples{},
      mDiff{}, mDiffSum{}, mPan{}, mDelayDuration{}, mDelayVolume{},
      mMasterVolume{1.0}, mMasterVolumeFactor{}, mPause{}, mIsDone{} {
  if (wave == nullptr) {
    return;
  }

  mWave = wave;
  mSourceChannels = mWave->Channels();
  mSourceSamplesPerSec = mWave->SamplesPerSec();
  mSourceBytesPerSample = mWave->BitsPerSample() / 8;
  mSourceTotalBytes = static_cast<int32_t>(mWave->DataLength());
  mSourceTotalSamples = mSourceTotalBytes / mSourceBytesPerSample;

  if (mPan == 0) {
    return;
  }
  if (mPan < 0) {
    mDelayChannel = 1;
  }
  if (mPan > 0) {
    mDelayChannel = 0;
  }

  const double maxSteps{20};

  if (mPan < -maxSteps) {
    mPan = maxSteps;
  }
  if (mPan > maxSteps) {
    mPan = -maxSteps;
  }

  const double maxDelayDuration{0.5}; /* ms */

  mDelayDuration = maxDelayDuration * abs(mPan) / maxSteps;
  mDelayVolume = 1.0 - abs(mPan) / 100.0;
}

WaveReader::~WaveReader() {}

void WaveReader::SetFormat(int16_t channels, int32_t samplesPerSec) {
  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;
  mDelaySamples = static_cast<int32_t>(
      static_cast<double>(mSourceSamplesPerSec) * mDelayDuration / 1000.0);
  mDiff = static_cast<double>(mSourceSamplesPerSec) /
          static_cast<double>(mTargetSamplesPerSec);
}

void WaveReader::Pause() { mMasterVolumeFactor = -0.1; }

void WaveReader::Restart() {
  mPause = false;
  mMasterVolumeFactor = 0.1;
}

bool WaveReader::IsPause() { return mPause; }

bool WaveReader::IsDone() {
  if (mIsDone) {
    return true;
  }

  mIsDone = static_cast<int32_t>(floor(mDiffSum)) * mSourceChannels >
            mSourceTotalSamples - 1;

  return mIsDone;
}

void WaveReader::Next() {
  if (mPause || IsDone()) {
    return;
  }

  mChannel = (mChannel + 1) % mTargetChannels;

  if (mChannel == 0) {
    mDiffSum += mDiff;
    mMasterVolume = mMasterVolume + mMasterVolumeFactor;
  }
  if (mMasterVolume > 1.0) {
    mMasterVolume = 1.0;
    mMasterVolumeFactor = 0.0;
  }
  if (mMasterVolume < 0.0) {
    mMasterVolume = 0.0;
    mMasterVolumeFactor = 0.0;

    mPause = true;
  }
}

int32_t WaveReader::Read() {
  if (mPause || IsDone()) {
    return 0;
  }

  double ratio = mDiffSum - floor(mDiffSum);
  int32_t base = static_cast<int32_t>(floor(mDiffSum)) * mSourceChannels;
  int32_t channel = mChannel % mSourceChannels;
  int32_t index1 =
      mSourceBytesPerSample * channel + mSourceBytesPerSample * base;
  int32_t index2 = mSourceBytesPerSample * channel +
                   mSourceBytesPerSample * (base + mSourceChannels);

  if (mChannel % 2 == mDelayChannel) {
    index1 = index1 - mDelaySamples * mSourceChannels * mSourceBytesPerSample;
    index2 = index2 - mDelaySamples * mSourceChannels * mSourceBytesPerSample;
  }

  int32_t value1 =
      index1 >= 0 && index1 < mSourceTotalBytes ? ReadInt32t(index1) : 0;
  int32_t value2 =
      index2 >= 0 && index2 < mSourceTotalBytes ? ReadInt32t(index2) : 0;

  double result = mMasterVolume * (value1 * (1.0 - ratio) + value2 * ratio);

  if (channel == mDelayChannel) {
    ratio = ratio * mDelayVolume;
  }

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
    : mTargetChannels{}, mTargetSamplesPerSec{}, mTargetTotalSamples{},
      mDiff{1.0}, mDiffSum{}, mDuration{duration}, mPause{}, mIsDone{} {}

SilentReader::~SilentReader() {}

void SilentReader::SetFormat(int16_t channels, int32_t samplesPerSec) {
  mDuration = mDuration - static_cast<double>(floor(mDiffSum)) /
                              static_cast<double>(samplesPerSec) * 1000.0;

  if (mDuration < 0.0) {
    mDuration = 0.0;
  }

  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;
  mTargetTotalSamples =
      static_cast<double>(mTargetChannels * mTargetSamplesPerSec) * mDuration /
      1000.0;
}

void SilentReader::Restart() { mPause = false; }

void SilentReader::Pause() { mPause = true; }

bool SilentReader::IsPause() { return mPause; }

bool SilentReader::IsDone() {
  if (mIsDone) {
    return true;
  }

  mIsDone = mDuration < 0.0 ||
            static_cast<int32_t>(floor(mDiffSum)) * mTargetChannels >
                mTargetTotalSamples - 1;

  return mIsDone;
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
