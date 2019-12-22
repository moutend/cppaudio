#include <cppaudio/engine.h>
#include <fstream>
#include <iostream>

int main() {
  std::ifstream input("/tmp/audio.wav", std::ios::binary | std::ios::in);
  std::streampos inputLength{};

  input.seekg(0, std::ios::end);
  inputLength = input.tellg();
  input.seekg(0, std::ios::beg);

  size_t bufferLength{};
  bufferLength = static_cast<size_t>(inputLength);
  char *buffer = new char[bufferLength]{};

  input.read(buffer, bufferLength);

  PCMAudio::RingEngine *engine = new PCMAudio::RingEngine();
  engine->SetTargetSamplesPerSec(44100);

  int bytesPerSample = 4;
  int samples = 44100 * 10;
  char *pData = new char[samples * bytesPerSample]{};

  for (int i = 0; i < samples; i++) {
    if (i % 11025 == 0) {
      engine->Feed(buffer, bufferLength);
    }

    double f64 = engine->Read();
    int32_t s32 = static_cast<int32_t>(f64);
    for (int j = 0; j < bytesPerSample; j++) {
      pData[bytesPerSample * i + bytesPerSample - 1 - j] =
          s32 >> (8 * (bytesPerSample - 1 - j)) & 0xFF;
    }

    engine->Next();
  }

  std::ofstream output("output.raw", std::ofstream::binary);
  output.write(pData, samples * bytesPerSample);
  output.close();

  delete engine;
  engine = nullptr;

  delete[] buffer;
  buffer = nullptr;

  input.close();

  return 0;
}
