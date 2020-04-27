#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

int main() {
  std::ifstream input("../assets/__INPUT_FILE__",
                      std::ios::binary | std::ios::in);

  int16_t outputBytesPerSample = 4;
  int16_t outputChannels = __OUTPUT_CHANNELS__;
  int32_t outputSamplesPerSec = __OUTPUT_SAMPLES_PER_SEC__;

  // Create an audio file whic duration is 10 sec.
  int32_t outputSamples = outputChannels * outputSamplesPerSec * 10;

  PCMAudio::LauncherEngine *engine = new PCMAudio::LauncherEngine(10, 32);
  engine->SetFormat(outputChannels, outputSamplesPerSec);
  engine->Register(0, input);

  input.close();
  char *pData = new char[outputSamples * outputBytesPerSample]{};

  for (int i = 0; i < outputSamples; i++) {
    if (engine->IsDone()) {
      engine->Feed(0);
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
