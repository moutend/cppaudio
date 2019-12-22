#include <cppaudio/engine.h>
#include <fstream>
#include <iostream>

int main() {
  PCMAudio::LauncherEngine *engine = new PCMAudio::LauncherEngine(10);

  std::streampos waveLength{};
  std::ifstream wav("/tmp/audio.wav", std::ios::binary | std::ios::in);

  wav.seekg(0, std::ios::end);
  waveLength = wav.tellg();
  wav.seekg(0, std::ios::beg);

  size_t bufferLength{};
  bufferLength = static_cast<size_t>(waveLength);
  char *buffer = new char[bufferLength]{+1};
  wav.read(buffer, bufferLength);

  if (!engine->Register(0, buffer, bufferLength)) {
    return -1;
  }

  delete engine;
  engine = nullptr;

  return 0;
}
