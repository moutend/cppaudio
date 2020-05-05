#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

// Kick Engine - Test pattern 33
//
// This test generates 30 seconds of audio.
//
// Timeline:
//
//  0.0s: Play inputA for 5 seconds.
//  5.0s: Play inputB for 5 seconds.
// 10.0s: Play inputA for 5 seconds.
// 15.0s: Play inputB for 5 seconds.
// 20.0s: Play inputA for 5 seconds.
// 25.0s: Play inputB for 5 seconds.
// 30.0s: Done.

int main() {
  std::ifstream inputA1("__INPUT_FILE__A.wav", std::ios::binary | std::ios::in);
  std::ifstream inputB1("__INPUT_FILE__B.wav", std::ios::binary | std::ios::in);
  std::ifstream inputA2("__INPUT_FILE__A.wav", std::ios::binary | std::ios::in);
  std::ifstream inputB2("__INPUT_FILE__B.wav", std::ios::binary | std::ios::in);

  if (!inputA1.is_open()) {
    printf("Failed to open '__INPUT_FILE__A1.wav'.\n");

    return -1;
  }
  if (!inputB1.is_open()) {
    printf("Failed to open '__INPUT_FILE__B1.wav'.\n");

    return -1;
  }
  if (!inputA2.is_open()) {
    printf("Failed to open '__INPUT_FILE__A2.wav'.\n");

    return -1;
  }
  if (!inputB2.is_open()) {
    printf("Failed to open '__INPUT_FILE__B2.wav'.\n");

    return -1;
  }

  std::streampos inputALength{};
  std::streampos inputBLength{};

  inputA2.seekg(0, std::ios::end);
  inputALength = inputA2.tellg();
  inputA2.seekg(0, std::ios::beg);

  inputB2.seekg(0, std::ios::end);
  inputBLength = inputB2.tellg();
  inputB2.seekg(0, std::ios::beg);

  size_t bufferALength{};
  bufferALength = static_cast<size_t>(inputALength);
  char *bufferA = new char[bufferALength]{};

  size_t bufferBLength{};
  bufferBLength = static_cast<size_t>(inputBLength);
  char *bufferB = new char[bufferBLength]{};

  inputA2.read(bufferA, bufferALength);
  inputB2.read(bufferB, bufferBLength);

  int16_t outputBytesPerSample = 4; // Always fixed to 32 bit.
  int16_t outputChannels = __OUTPUT_CHANNELS__;
  int32_t outputSamplesPerSec = __OUTPUT_SAMPLES_PER_SEC__;
  int32_t outputSamples = outputChannels * outputSamplesPerSec * 30;

  char *pData = new char[outputSamples * outputBytesPerSample]{};

  PCMAudio::KickEngine *engine = new PCMAudio::KickEngine(2, 32);
  engine->SetFormat(outputChannels, outputSamplesPerSec);
  engine->Register(0, inputA1);
  engine->Register(1, inputB1);

  int16_t index{};

  for (int i = 0; i < outputSamples; i++) {
    if (engine->IsDone()) {
      switch (index % 2) {
      case 0:
        engine->Kick(0);
        break;
      case 1:
        engine->Kick(bufferB, bufferBLength);
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

  inputA1.close();
  inputB1.close();
  inputA2.close();
  inputB2.close();

  delete engine;
  engine = nullptr;

  return 0;
}
