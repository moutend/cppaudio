#include <cppaudio/engine.h>

#include <mutex>

namespace PCMAudio {
LauncherEngine::LauncherEngine(int16_t maxWaves, int16_t maxReaders)
    : mWaves(nullptr), mReaders(nullptr), mScheduledReaders(nullptr),
      mTargetChannels(0), mTargetSamplesPerSec(0), mChannel(0), mIndex(0),
      mMaxWaves(maxWaves), mMaxReaders(maxReaders) {
  mWaves = new Wave *[mMaxWaves] {};
  mReaders = new Reader *[mMaxReaders] {};
  mScheduledReaders = new ReaderInfo *[mMaxReaders] {};
}

LauncherEngine::~LauncherEngine() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxWaves; i++) {
    if (mWaves[i] != nullptr) {
      delete mWaves[i];
      mWaves[i] = nullptr;
    }
  }

  delete[] mWaves;
  mWaves = nullptr;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      delete mReaders[i];
      mReaders[i] = nullptr;
    }
    if (mScheduledReaders[i] != nullptr) {
      delete mScheduledReaders[i];
      mScheduledReaders[i] = nullptr;
    }
  }

  delete[] mReaders;
  mReaders = nullptr;

  delete[] mScheduledReaders;
  mScheduledReaders = nullptr;
}

void LauncherEngine::Reset() {}

void LauncherEngine::SetFormat(int16_t channels, int32_t samplesPerSec) {
  std::lock_guard<std::mutex> guard(mMutex);

  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->SetFormat(channels, samplesPerSec);
    }
  }
}

void LauncherEngine::Restart() {
  std::lock_guard<std::mutex> guard(mMutex);

  int16_t index = (mIndex + mMaxReaders - 1) % mMaxReaders;

  if (mReaders[index] != nullptr) {
    mReaders[index]->Restart();
  }
}

void LauncherEngine::Pause() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }
}

bool LauncherEngine::IsDone() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mScheduledReaders[i] != nullptr) {
      return false;
    }
  }
  if (mReaders[(mIndex + mMaxReaders - 1) % mMaxReaders] != nullptr) {
    return false;
  }

  return true;
}

void LauncherEngine::Next() {
  std::lock_guard<std::mutex> guard(mMutex);

  mChannel = (mChannel + 1) % mTargetChannels;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mScheduledReaders[i] != nullptr) {
      if (mScheduledReaders[i]->DelayCount == 0) {
        if (mScheduledReaders[i]->SleepDuration > 0.0) {
          delete mReaders[mIndex];
          mReaders[mIndex] =
              new SilentReader(mScheduledReaders[i]->SleepDuration);
          mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
          mIndex = (mIndex + 1) % mMaxReaders;
        } else {
          for (int16_t j = 0; j < mMaxReaders; j++) {
            if (mReaders[j] != nullptr) {
              mReaders[j]->Pause();
            }
          }

          delete mReaders[mIndex];
          mReaders[mIndex] =
              new WaveReader(mWaves[mScheduledReaders[i]->WaveIndex]);
          mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
          mIndex = (mIndex + 1) % mMaxReaders;
        }

        delete mScheduledReaders[i];
        mScheduledReaders[i] = nullptr;
      } else {
        mScheduledReaders[i]->DelayCount -= 1;
      }
    }
    if (mReaders[i] != nullptr) {
      mReaders[i]->Next();

      if (mReaders[i]->IsDone()) {
        delete mReaders[i];
        mReaders[i] = nullptr;
      }
    }
  }
}

void LauncherEngine::Sleep(double sleepDuration /* ms */) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (sleepDuration <= 0.0) {
    return;
  }
  if (mChannel == 0) {
    mReaders[mIndex] = new SilentReader(sleepDuration);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];

    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->SleepDuration = sleepDuration;
    mScheduledReaders[mIndex]->WaveIndex = -1;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
}

double LauncherEngine::Read() {
  std::lock_guard<std::mutex> guard(mMutex);

  double result = 0.0;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      result += mReaders[i]->Read();
    }
  }

  return result;
}

void LauncherEngine::Start(int16_t waveIndex) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (waveIndex < 0 || waveIndex > mMaxWaves - 1 ||
      mWaves[waveIndex] == nullptr) {
    return;
  }
  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }
  if (mChannel == 0) {
    delete mReaders[mIndex];
    mReaders[mIndex] = new WaveReader(mWaves[waveIndex]);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];

    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->SleepDuration = 0.0;
    mScheduledReaders[mIndex]->WaveIndex = waveIndex;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
}

void LauncherEngine::Register(int16_t waveIndex, std::istream &input) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (waveIndex < 0 || waveIndex > mMaxWaves - 1) {
    return;
  }

  delete mWaves[waveIndex];
  mWaves[waveIndex] = nullptr;
  mWaves[waveIndex] = new Wave(input);

  if (!mWaves[waveIndex]->HasLoaded()) {
    delete mWaves[waveIndex];
    mWaves[waveIndex] = nullptr;
  }
}

RingEngine::RingEngine(int16_t maxReaders)
    : mWaves(nullptr), mReaders(nullptr), mScheduledReaders(0),
      mMaxReaders(maxReaders), mTargetChannels(0), mTargetSamplesPerSec(0),
      mChannel(0), mIndex(0), mWaveIndex(0) {
  mWaves = new Wave *[mMaxReaders] {};
  mReaders = new Reader *[mMaxReaders] {};
  mScheduledReaders = new ReaderInfo *[mMaxReaders] {};
}

RingEngine::~RingEngine() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mWaves[i] != nullptr) {
      delete mWaves[i];
      mWaves[i] = nullptr;
    }
    if (mReaders[i] != nullptr) {
      delete mReaders[i];
      mReaders[i] = nullptr;
    }
    if (mScheduledReaders[i] != nullptr) {
      delete mScheduledReaders[i];
      mScheduledReaders = nullptr;
    }
  }

  delete[] mWaves;
  mWaves = nullptr;

  delete[] mReaders;
  mReaders = nullptr;

  delete[] mScheduledReaders;
  mScheduledReaders = nullptr;
}

void RingEngine::Reset() {}

void RingEngine::SetFormat(int16_t channels, int32_t samplesPerSec) {
  std::lock_guard<std::mutex> guard(mMutex);

  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->SetFormat(channels, samplesPerSec);
    }
  }
}

void RingEngine::Restart() {
  std::lock_guard<std::mutex> guard(mMutex);

  int16_t index = (mIndex + mMaxReaders - 1) % mMaxReaders;

  if (mReaders[index] != nullptr) {
    mReaders[index]->Restart();
  }
}

void RingEngine::Pause() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }
}

bool RingEngine::IsDone() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mScheduledReaders[i] != nullptr) {
      return false;
    }
  }
  if (mReaders[(mIndex + mMaxReaders - 1) % mMaxReaders] != nullptr) {
    return false;
  }

  return true;
}

void RingEngine::Next() {
  std::lock_guard<std::mutex> guard(mMutex);

  mChannel = (mChannel + 1) % mTargetChannels;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mScheduledReaders[i] != nullptr) {
      if (mScheduledReaders[i]->DelayCount == 0) {
        if (mScheduledReaders[i]->SleepDuration > 0.0) {
          mReaders[mIndex] =
              new SilentReader(mScheduledReaders[i]->SleepDuration);
          mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
          mIndex = (mIndex + 1) % mMaxReaders;
        } else {
          for (int16_t j = 0; j < mMaxReaders; j++) {
            if (mReaders[j] != nullptr) {
              mReaders[j]->Pause();
            }
          }
          mReaders[mIndex] =
              new WaveReader(mWaves[mScheduledReaders[i]->WaveIndex]);
          mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
          mIndex = (mIndex + 1) % mMaxReaders;
        }

        delete mScheduledReaders[i];
        mScheduledReaders[i] = nullptr;
      } else {
        mScheduledReaders[i]->DelayCount -= 1;
      }
    }
    if (mReaders[i] != nullptr) {
      mReaders[i]->Next();

      if (mReaders[i]->IsDone()) {
        delete mReaders[i];
        mReaders[i] = nullptr;
      }
    }
  }
}

void RingEngine::Sleep(double sleepDuration /* ms */) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (sleepDuration <= 0.0) {
    return;
  }
  if (mChannel == 0) {
    mReaders[mIndex] = new SilentReader(sleepDuration);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];
    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->SleepDuration = sleepDuration;
    mScheduledReaders[mIndex]->WaveIndex = -1;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
}

double RingEngine::Read() {
  std::lock_guard<std::mutex> guard(mMutex);

  double result = 0.0;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      result += mReaders[i]->Read();
    }
  }

  return result;
}

void RingEngine::Start(char *buffer, int32_t bufferLength) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (buffer == nullptr || bufferLength <= 0) {
    return;
  }
  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }

  delete mWaves[mWaveIndex];
  mWaves[mWaveIndex] = new Wave(buffer, bufferLength);

  if (mChannel == 0) {
    delete mReaders[mIndex];
    mReaders[mIndex] = new WaveReader(mWaves[mWaveIndex]);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];
    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->SleepDuration = 0.0;
    mScheduledReaders[mIndex]->WaveIndex = mWaveIndex;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
  mWaveIndex = (mWaveIndex + 1) % mMaxReaders;
}
} // namespace PCMAudio
