#include <cppaudio/engine.h>
#include <fstream>
#include <iostream>

int main() {
  std::ifstream input("../assets/audio.wav", std::ios::binary | std::ios::in);

  PCMAudio::LauncherEngine *engine = new PCMAudio::LauncherEngine(10);
  engine->SetTargetSamplesPerSec(44100);

  if (!engine->Register(0, input)) {
    return -1;
  }

  input.close();

  int bytesPerSample = 4;
  int samples = 44100 * 10;
  char *pData = new char[samples * bytesPerSample]{};

  for (int i = 0; i < samples; i++) {
    if (i % 11025 == 0) {
      engine->Feed(0);
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

  return 0;
}
