#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

int main() {
  std::ifstream input("__INPUT_FILE__");

  if (!input.is_open()) {
    printf("Failed to open '__INPUT_FILE__'.\n");

    return -1;
  }

  std::streampos inputLength{};

  input.seekg(0, std::ios::end);
  inputLength = input.tellg();
  input.seekg(0, std::ios::beg);

  size_t bufferLength{};
  bufferLength = static_cast<size_t>(inputLength);
  char *buffer = new char[bufferLength]{};

  input.read(buffer, bufferLength);
  input.close();

  int16_t outputBytesPerSample = 4; // Always fixed to 32 bit.
  int16_t outputChannels = __OUTPUT_CHANNELS__;
  int32_t outputSamplesPerSec = __OUTPUT_SAMPLES_PER_SEC__;

  // Create an audio file whic duration is 3 sec.
  int32_t outputSamples = outputChannels * outputSamplesPerSec * 3;
  char *pData = new char[outputSamples * outputBytesPerSample]{};

  PCMAudio::RingEngine *engine = new PCMAudio::RingEngine();
  engine->SetFormat(outputChannels, outputSamplesPerSec);

  for (int i = 0; i < outputSamples; i++) {
    if (i == 0) {
      engine->Feed(buffer, bufferLength);
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

  delete[] buffer;
  buffer = nullptr;

  return 0;
}
