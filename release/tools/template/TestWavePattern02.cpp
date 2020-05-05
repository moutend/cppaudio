#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

// Wave - Test pattern 2
//
// This test prints given wave audio file format.

int main() {
  std::ifstream inputA("__INPUT_FILE__A.wav", std::ios::binary | std::ios::in);

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

  PCMAudio::Wave *wav = new PCMAudio::Wave(bufferA, bufferALength);

  if (wav == nullptr) {
    return -1;
  }

  printf("Channels: %d\n", wav->Channels());
  printf("Samples per sec: %d\n", wav->SamplesPerSec());
  printf("Bits per sample: %d\n", wav->BitsPerSample());
  printf("Data length (bytes): %d\n", wav->DataLength());

  delete[] bufferA;
  bufferA = nullptr;

  return 0;
}
