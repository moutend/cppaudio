#include <cppaudio/engine.h>

#include <mutex>

namespace PCMAudio {
KickEngine::KickEngine(int16_t maxRegisteredWaves, int16_t maxReaders)
    : mTemporaryWaves(nullptr), mRegisteredWaves(nullptr), mReaders(nullptr),
      mScheduledReaders(nullptr), mTargetChannels(0), mTargetSamplesPerSec(0),
      mChannel(0), mIndex(0), mTemporaryWaveIndex(0),
      mMaxTemporaryWaves(maxReaders), mMaxRegisteredWaves(maxRegisteredWaves),
      mMaxReaders(maxReaders) {
  mTemporaryWaves = new Wave *[mMaxTemporaryWaves] {};
  mRegisteredWaves = new Wave *[mMaxRegisteredWaves] {};
  mReaders = new Reader *[mMaxReaders] {};
  mScheduledReaders = new ReaderInfo *[mMaxReaders] {};
}

KickEngine::~KickEngine() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxRegisteredWaves; i++) {
    if (mRegisteredWaves[i] != nullptr) {
      delete mRegisteredWaves[i];
      mRegisteredWaves[i] = nullptr;
    }
  }

  delete[] mRegisteredWaves;
  mRegisteredWaves = nullptr;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mTemporaryWaves[i] != nullptr) {
      delete mTemporaryWaves[i];
      mTemporaryWaves[i] = nullptr;
    }
    if (mReaders[i] != nullptr) {
      delete mReaders[i];
      mReaders[i] = nullptr;
    }
    if (mScheduledReaders[i] != nullptr) {
      delete mScheduledReaders[i];
      mScheduledReaders[i] = nullptr;
    }
  }

  delete[] mTemporaryWaves;
  mTemporaryWaves = nullptr;

  delete[] mReaders;
  mReaders = nullptr;

  delete[] mScheduledReaders;
  mScheduledReaders = nullptr;
}

void KickEngine::SetFormat(int16_t channels, int32_t samplesPerSec) {
  std::lock_guard<std::mutex> guard(mMutex);

  mTargetChannels = channels;
  mTargetSamplesPerSec = samplesPerSec;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->SetFormat(channels, samplesPerSec);
    }
  }
}

void KickEngine::Restart() {
  std::lock_guard<std::mutex> guard(mMutex);

  int16_t index = (mIndex + mMaxReaders - 1) % mMaxReaders;

  if (mReaders[index] != nullptr) {
    mReaders[index]->Restart();
  }
}

void KickEngine::Pause() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }
}

bool KickEngine::IsDone() {
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

void KickEngine::Next() {
  std::lock_guard<std::mutex> guard(mMutex);

  mChannel = (mChannel + 1) % mTargetChannels;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mScheduledReaders[i] != nullptr) {
      if (mScheduledReaders[i]->DelayCount > 0) {
        mScheduledReaders[i]->DelayCount -= 1;
        continue;
      }
      switch (mScheduledReaders[i]->Type) {
      case 1:
        delete mReaders[mIndex];
        mReaders[mIndex] =
            new SilentReader(mScheduledReaders[i]->SleepDuration);
        break;
      case 2:
        for (int16_t j = 0; j < mMaxReaders; j++) {
          if (mReaders[j] != nullptr) {
            mReaders[j]->Pause();
          }
        }

        delete mReaders[mIndex];
        mReaders[mIndex] = new WaveReader(
            mRegisteredWaves[mScheduledReaders[i]->RegisteredWaveIndex]);
        break;
      case 3:
        for (int16_t j = 0; j < mMaxReaders; j++) {
          if (mReaders[j] != nullptr) {
            mReaders[j]->Pause();
          }
        }

        delete mReaders[mIndex];
        mReaders[mIndex] = new WaveReader(
            mTemporaryWaves[mScheduledReaders[i]->TemporaryWaveIndex]);
        break;
      }

      mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
      mIndex = (mIndex + 1) % mMaxReaders;

      delete mScheduledReaders[i];
      mScheduledReaders[i] = nullptr;
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

void KickEngine::Sleep(double sleepDuration /* ms */) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (sleepDuration <= 0.0) {
    return;
  }
  if (mChannel == 0) {
    delete mReaders[mIndex];
    mReaders[mIndex] = new SilentReader(sleepDuration);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];
    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->Type = 1;
    mScheduledReaders[mIndex]->SleepDuration = sleepDuration;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
}

double KickEngine::Read() {
  std::lock_guard<std::mutex> guard(mMutex);

  double result = 0.0;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      result += mReaders[i]->Read();
    }
  }

  return result;
}

void KickEngine::Register(int16_t waveIndex, std::istream &input) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (waveIndex < 0 || waveIndex > mMaxRegisteredWaves - 1) {
    return;
  }

  delete mRegisteredWaves[waveIndex];
  mRegisteredWaves[waveIndex] = new Wave(input);

  if (!mRegisteredWaves[waveIndex]->HasLoaded()) {
    delete mRegisteredWaves[waveIndex];
    mRegisteredWaves[waveIndex] = nullptr;
  }
}

void KickEngine::Kick(int16_t waveIndex) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (waveIndex < 0 || waveIndex > mMaxRegisteredWaves - 1 ||
      mRegisteredWaves[waveIndex] == nullptr) {
    return;
  }
  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }
  if (mChannel == 0) {
    delete mReaders[mIndex];
    mReaders[mIndex] = new WaveReader(mRegisteredWaves[waveIndex]);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];
    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->Type = 2;
    mScheduledReaders[mIndex]->RegisteredWaveIndex = waveIndex;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
}

void KickEngine::Kick(char *buffer, int32_t bufferLength) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (buffer == nullptr || bufferLength <= 0) {
    return;
  }
  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Pause();
    }
  }

  delete mTemporaryWaves[mTemporaryWaveIndex];
  mTemporaryWaves[mTemporaryWaveIndex] = new Wave(buffer, bufferLength);

  if (mChannel == 0) {
    delete mReaders[mIndex];
    mReaders[mIndex] = new WaveReader(mTemporaryWaves[mTemporaryWaveIndex]);
    mReaders[mIndex]->SetFormat(mTargetChannels, mTargetSamplesPerSec);
  } else {
    delete mScheduledReaders[mIndex];
    mScheduledReaders[mIndex] = new ReaderInfo;
    mScheduledReaders[mIndex]->Type = 3;
    mScheduledReaders[mIndex]->TemporaryWaveIndex = mTemporaryWaveIndex;
    mScheduledReaders[mIndex]->DelayCount = mTargetChannels - mChannel;
  }

  mIndex = (mIndex + 1) % mMaxReaders;
  mTemporaryWaveIndex = (mTemporaryWaveIndex + 1) % mMaxReaders;
}
} // namespace PCMAudio
