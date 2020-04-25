#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

// Launcher Engine - Test pattern 3
//
// This test generates 7 seconds of audio.
//
// Timeline:
//
// 0.0s: Play inputA for 0.5 seconds.
// 0.5s: Play inputB for 5 seconds.
// 5.5s: Play inputA for 1.5 seconds.
// 7.0s: Done.

int main() {
  std::ifstream inputA("__INPUT_FILE__A.wav", std::ios::binary | std::ios::in);
  std::ifstream inputB("__INPUT_FILE__B.wav", std::ios::binary | std::ios::in);

  if (!inputA.is_open()) {
    printf("Failed to open '__INPUT_FILE__A.wav'.\n");

    return -1;
  }
  if (!inputB.is_open()) {
    printf("Failed to open '__INPUT_FILE__B.wav'.\n");

    return -1;
  }

  int16_t outputBytesPerSample = 4; // Always fixed to 32 bit.
  int16_t outputChannels = __OUTPUT_CHANNELS__;
  int32_t outputSamplesPerSec = __OUTPUT_SAMPLES_PER_SEC__;
  int32_t outputSamples = outputChannels * outputSamplesPerSec * 7;

  char *pData = new char[outputSamples * outputBytesPerSample]{};

  PCMAudio::LauncherEngine *engine = new PCMAudio::LauncherEngine(2, 32);
  engine->SetFormat(outputChannels, outputSamplesPerSec);
  engine->Register(0, inputA);
  engine->Register(1, inputB);

  inputA.close();
  inputB.close();

  int16_t index{};

  for (int i = 0; i < outputSamples; i++) {
    if (i == 0 || i == outputSamplesPerSec * outputChannels / 2 ||
        engine->IsDone()) {
      engine->Start(index % 2);
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

  return 0;
}
