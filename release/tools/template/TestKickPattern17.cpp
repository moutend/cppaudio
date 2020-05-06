#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

// Kick Engine - Test pattern 17
//
// This test generates 7 seconds of audio.
//
// Timeline:
//
// 0.0s: Play silence for 0.5 seconds.
// 0.5s: Play inputA for 5 seconds.
// 5.5s: Play silence for 1.5 seconds.
// 7.0s: Done.

int main() {
  std::ifstream inputA("__INPUT_FILE__A.wav");

  if (!inputA.is_open()) {
    printf("Failed to open '__INPUT_FILE__A.wav'.\n");

    return -1;
  }

  std::streampos inputALength{};

  inputA.seekg(0, std::ios::end);
  inputALength = inputA.tellg();
  inputA.seekg(0, std::ios::beg);

  size_t bufferALength{};
  bufferALength = static_cast<size_t>(inputALength);
  char *bufferA = new char[bufferALength]{};

  inputA.read(bufferA, bufferALength);
  inputA.close();

  int16_t outputBytesPerSample = 4; // Always fixed to 32 bit.
  int16_t outputChannels = __OUTPUT_CHANNELS__;
  int32_t outputSamplesPerSec = __OUTPUT_SAMPLES_PER_SEC__;
  int32_t outputSamples = outputChannels * outputSamplesPerSec * 7;

  char *pData = new char[outputSamples * outputBytesPerSample]{};

  PCMAudio::KickEngine *engine = new PCMAudio::KickEngine(2, 32);
  engine->SetFormat(outputChannels, outputSamplesPerSec);

  for (int i = 0; i < outputSamples; i++) {
    if (i == outputSamplesPerSec * outputChannels / 2) {
      engine->Kick(bufferA, bufferALength);
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

  return 0;
}