#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

// Ring Engine - Test pattern 14
//
// This test generates 8 seconds of audio.
//
// Timeline:
//
// 0.0s: Play inputA for 1 seconds plus 1 sample.
// 1.0s: Play inputB for 5 seconds.
// 6.0s: Play inputA for 2 seconds.
// 8.0s: Done.

int main() {
  std::ifstream inputA("__INPUT_FILE__A.wav");
  std::ifstream inputB("__INPUT_FILE__B.wav");

  if (!inputA.is_open()) {
    printf("Failed to open '__INPUT_FILE__A.wav'.\n");

    return -1;
  }
  if (!inputB.is_open()) {
    printf("Failed to open '__INPUT_FILE__B.wav'.\n");

    return -1;
  }

  std::streampos inputALength{};
  std::streampos inputBLength{};

  inputA.seekg(0, std::ios::end);
  inputALength = inputA.tellg();
  inputA.seekg(0, std::ios::beg);

  inputB.seekg(0, std::ios::end);
  inputBLength = inputB.tellg();
  inputB.seekg(0, std::ios::beg);

  size_t bufferALength{};
  bufferALength = static_cast<size_t>(inputALength);
  char *bufferA = new char[bufferALength]{};

  size_t bufferBLength{};
  bufferBLength = static_cast<size_t>(inputBLength);
  char *bufferB = new char[bufferBLength]{};

  inputA.read(bufferA, bufferALength);
  inputA.close();

  inputB.read(bufferB, bufferBLength);
  inputB.close();

  int16_t outputBytesPerSample = 4; // Always fixed to 32 bit.
  int16_t outputChannels = __OUTPUT_CHANNELS__;
  int32_t outputSamplesPerSec = __OUTPUT_SAMPLES_PER_SEC__;
  int32_t outputSamples = outputChannels * outputSamplesPerSec * 8;

  char *pData = new char[outputSamples * outputBytesPerSample]{};

  PCMAudio::RingEngine *engine = new PCMAudio::RingEngine(32);
  engine->SetFormat(outputChannels, outputSamplesPerSec);

  int16_t index{};

  for (int i = 0; i < outputSamples; i++) {
    if (engine->IsDone() || i == (outputSamplesPerSec * outputChannels + 1)) {
      switch (index % 2) {
      case 0:
        engine->Start(bufferA, bufferALength);
        break;
      case 1:
        engine->Start(bufferB, bufferBLength);
        break;
      }
      index += 1;
    }

    int32_t s32 = engine->Read();

    for (int j = 0; j < outputBytesPerSample; j++) {
      pData[outputBytesPerSample * i + outputBytesPerSample - 1 - j] =
          s32 >> (8 * (outputBytesPerSample - 1 - j)) & 0xFF;
    }

    engine->Next();
  }

  std::ofstream output("output.raw", std::ofstream::binary);
  output.write(pData, outputSamples * outputBytesPerSample);
  output.close();

  delete engine;
  engine = nullptr;

  delete[] bufferA;
  bufferA = nullptr;

  delete[] bufferB;
  bufferB = nullptr;

  return 0;
}
