#pragma once

#include <cstdint>
#include <iostream>
#include <mutex>

#include "reader.h"
#include "wave.h"

namespace PCMAudio {
class Engine {
public:
  virtual void Reset() = 0;
  virtual void SetFormat(int16_t channels, int32_t samplesPerSec) = 0;
  virtual void FadeIn() = 0;
  virtual void FadeOut() = 0;
  virtual bool IsDone() = 0;
  virtual void Next() = 0;
  virtual double Read() = 0;
};

class LauncherEngine : public Engine {
public:
  LauncherEngine(int16_t maxWaves, int16_t maxReaders);
  ~LauncherEngine();

  void Reset();
  virtual void SetFormat(int16_t channels, int32_t samplesPerSec);
  void FadeIn();
  void FadeOut();
  bool IsDone();
  void Next();
  double Read();

  void Sleep(double duration /* ms */);
  void Feed(int16_t waveIndex);
  void Register(int16_t waveIndex, std::istream &input);

private:
  std::mutex mMutex;

  Wave **mWaves;
  Reader **mReaders;

  int16_t mTargetChannels = 2;
  int32_t mTargetSamplesPerSec;

  int16_t mChannel;
  int16_t mIndex;
  int16_t mMaxWaves;
  int16_t mMaxReaders;
};

class RingEngine : public Engine {
public:
  RingEngine();
  ~RingEngine();

  void Reset();
  virtual void SetFormat(int16_t channels, int32_t samplesPerSec);
  void FadeIn();
  void FadeOut();
  bool IsDone();
  void Next();
  double Read();

  void Feed(char *buffer, int32_t bufferLength);

private:
  std::mutex mMutex;

  Wave **mWaves;
  Reader **mReaders;

  int16_t mTargetChannels;
  int32_t mTargetSamplesPerSec;

  int16_t mChannel;
  int16_t mMaxReaders;
  int16_t mIndex;
};
} // namespace PCMAudio
