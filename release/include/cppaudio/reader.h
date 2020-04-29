#pragma once

#include <cstdint>

#include "wave.h"

namespace PCMAudio {
class Reader {
public:
  virtual ~Reader(){};

  virtual void SetFormat(int16_t channels, int32_t samplesPerSec) = 0;
  virtual void FadeIn() = 0;
  virtual void FadeOut() = 0;
  virtual bool IsDone() = 0;
  virtual void Next() = 0;
  virtual int32_t Read() = 0;
};

class WaveReader : public Reader {
public:
  WaveReader(Wave *&wave);
  ~WaveReader();

  void SetFormat(int16_t channels, int32_t samplesPerSec);
  void FadeIn();
  void FadeOut();
  bool IsDone();
  void Next();
  int32_t Read();

  double ReadDouble(double index);
  int32_t ReadInt32t(int32_t index);

private:
  Wave *mWave;

  int16_t mTargetChannels;
  int32_t mTargetSamplesPerSec;

  int16_t mSourceChannels;
  int32_t mSourceSamplesPerSec;
  int16_t mSourceBytesPerSample;
  int32_t mSourceTotalBytes;
  int32_t mSourceTotalSamples;

  int16_t mChannel;
  double mDiff;
  double mDiffSum;
  double mVolume;
  double mVolumeFactor;

  bool mPause;
};

class SilentReader : public Reader {
public:
  SilentReader(double duration /* ms*/);
  ~SilentReader();

  void SetFormat(int16_t channels, int32_t samplesPerSec);
  void FadeIn();
  void FadeOut();
  bool IsDone();
  void Next();
  int32_t Read();

private:
  int16_t mTargetChannels;
  int32_t mTargetSamplesPerSec;

  int32_t mSourceTotalSamples;
  int32_t mSampleSum;

  double mDuration;
};
} // namespace PCMAudio
