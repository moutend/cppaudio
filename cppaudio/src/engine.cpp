#include <cppaudio/engine.h>
#include <mutex>

namespace PCMAudio {
LauncherEngine::LauncherEngine(int16_t maxWaves) {
  mMaxWaves = maxWaves;
  mWaves = new Wave *[mMaxWaves] {};
  mReaders = new Reader *[mMaxReaders] {};
}

LauncherEngine::~LauncherEngine() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxWaves; i++) {
    delete mWaves[i];
    mWaves[i] = nullptr;
  }

  delete[] mWaves;
  mWaves = nullptr;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    delete mReaders[i];
    mReaders[i] = nullptr;
  }

  delete[] mReaders;
  mReaders = nullptr;
}

void LauncherEngine::Reset() {
  std::lock_guard<std::mutex> guard(mMutex);

  mCompleted = false;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    mReaders[i] = nullptr;
  }
}

void LauncherEngine::SetTargetSamplesPerSec(int32_t samples) {
  std::lock_guard<std::mutex> guard(mMutex);

  mTargetSamplesPerSec = samples;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->SetTargetSamplesPerSec(mTargetSamplesPerSec);
    }
  }
}

void LauncherEngine::FadeIn() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->FadeIn();
    }
  }
}

void LauncherEngine::FadeOut() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->FadeOut();
    }
  }
}

bool LauncherEngine::IsCompleted() { return mCompleted; }

void LauncherEngine::Next() {
  std::lock_guard<std::mutex> guard(mMutex);

  mCurrentChannel = (mCurrentChannel + 1) % 2;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Next();
      mCompleted = mCompleted | mReaders[i]->IsCompleted();
    }
  }
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

bool LauncherEngine::Sleep(double duration /* ms */) {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->FadeOut();
    }
  }

  mReaders[mIndex] = new SilentReader(mTargetSamplesPerSec, duration);

  mIndex = (mIndex + 1) % mMaxReaders;
  mCompleted = false;

  return true;
}

bool LauncherEngine::Feed(int16_t waveIndex) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (waveIndex < 0 || waveIndex > mMaxWaves - 1) {
    return false;
  }
  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->FadeOut();
    }
  }

  mReaders[mIndex] = new WaveReader(mWaves[waveIndex], mCurrentChannel);
  mReaders[mIndex]->SetTargetSamplesPerSec(mTargetSamplesPerSec);

  mIndex = (mIndex + 1) % mMaxReaders;
  mCompleted = false;

  return true;
}

bool LauncherEngine::Register(int16_t waveIndex, const char *buffer,
                              size_t bufferLength) {
  std::lock_guard<std::mutex> guard(mMutex);

  if (waveIndex < 0 || waveIndex > mMaxWaves - 1) {
    return false;
  }

  delete mWaves[waveIndex];
  mWaves[waveIndex] = nullptr;
  mWaves[waveIndex] = new Wave(buffer, bufferLength);

  return true;
}

RingEngine::RingEngine() {
  mWaves = new Wave *[mMaxReaders] {};
  mReaders = new Reader *[mMaxReaders] {};
}

RingEngine::~RingEngine() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    delete mWaves[i];
    mWaves[i] = nullptr;
  }

  delete[] mWaves;
  mWaves = nullptr;

  delete[] mReaders;
  mReaders = nullptr;
}

void RingEngine::Reset() {
  std::lock_guard<std::mutex> guard(mMutex);

  mCompleted = false;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    mReaders[i] = nullptr;
  }
}

void RingEngine::SetTargetSamplesPerSec(int32_t samples) {
  std::lock_guard<std::mutex> guard(mMutex);

  mTargetSamplesPerSec = samples;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->SetTargetSamplesPerSec(mTargetSamplesPerSec);
    }
  }
}

void RingEngine::FadeIn() {
  std::lock_guard<std::mutex> guard(mMutex);

  int16_t i = mIndex - 1 > 0 ? mIndex - 1 : mMaxReaders - 1;

  if (mReaders[i] != nullptr) {
    mReaders[i]->FadeIn();
  }
}

void RingEngine::FadeOut() {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->FadeOut();
    }
  }
}

void RingEngine::Feed(char *buffer, int32_t bufferLength) {
  std::lock_guard<std::mutex> guard(mMutex);

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->FadeOut();
    }
  }

  delete mWaves[mIndex];
  mWaves[mIndex] = nullptr;
  mWaves[mIndex] = new Wave(buffer, bufferLength);

  mReaders[mIndex] = new WaveReader(mWaves[mIndex], mCurrentChannel);
  mReaders[mIndex]->SetTargetSamplesPerSec(mTargetSamplesPerSec);

  mIndex = (mIndex + 1) % mMaxReaders;
  mCompleted = false;
}

bool RingEngine::IsCompleted() { return mCompleted; }

void RingEngine::Next() {
  std::lock_guard<std::mutex> guard(mMutex);

  mCurrentChannel = (mCurrentChannel + 1) % 2;

  for (int16_t i = 0; i < mMaxReaders; i++) {
    if (mReaders[i] != nullptr) {
      mReaders[i]->Next();
      mCompleted = mCompleted | mReaders[i]->IsCompleted();
    }
  }
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
} // namespace PCMAudio
