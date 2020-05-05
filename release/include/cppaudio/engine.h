#pragma once

#include <cstdint>
#include <iostream>
#include <mutex>

#include "reader.h"
#include "wave.h"

struct ReaderInfo {
  int16_t Type;
  int16_t RegisteredWaveIndex;
  int16_t TemporaryWaveIndex;
  double SleepDuration;
  int16_t WaveIndex;
  int16_t DelayCount;
};

namespace PCMAudio {
class Engine {
public:
  virtual void SetFormat(int16_t channels, int32_t samplesPerSec) = 0;
  virtual void Restart() = 0;
  virtual void Pause() = 0;
  virtual bool IsDone() = 0;
  virtual void Next() = 0;
  virtual void Sleep(double duration /* ms */) = 0;
  virtual double Read() = 0;
};

class KickEngine : public Engine {
public:
  KickEngine(int16_t maxRegisteredWaves, int16_t maxReaders);
  ~KickEngine();

  void SetFormat(int16_t channels, int32_t samplesPerSec);
  void Restart();
  void Pause();
  bool IsDone();
  void Next();
  void Sleep(double duration /* ms */);
  double Read();

  void Register(int16_t waveIndex, std::istream &input);
  void Kick(int16_t waveIndex);
  void Kick(char *buffer, int32_t bufferLength);

private:
  std::mutex mMutex;

  Wave **mTemporaryWaves;
  Wave **mRegisteredWaves;
  Reader **mReaders;
  ReaderInfo **mScheduledReaders;

  int16_t mTargetChannels;
  int32_t mTargetSamplesPerSec;

  int16_t mChannel;
  int16_t mIndex;
  int16_t mTemporaryWaveIndex;

  int16_t mMaxTemporaryWaves;
  int16_t mMaxRegisteredWaves;
  int16_t mMaxReaders;
};
} // namespace PCMAudio
