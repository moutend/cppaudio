#include <cppaudio/engine.h>
#include <cstdint>
#include <fstream>
#include <iostream>

// Wave - Test pattern 1
//
// This test prints given wave audio file format.

int main() {
  std::ifstream inputA("__INPUT_FILE__A.wav", std::ios::binary | std::ios::in);

  if (!inputA.is_open()) {
    printf("Failed to open '__INPUT_FILE__A.wav'.\n");

    return -1;
  }

  PCMAudio::Wave *wav = new PCMAudio::Wave(inputA);

  if (wav == nullptr) {
    return -1;
  }

  inputA.close();

  printf("Channels: %d\n", wav->Channels());
  printf("Samples per sec: %d\n", wav->SamplesPerSec());
  printf("Bits per sample: %d\n", wav->BitsPerSample());
  printf("Data length (bytes): %d\n", wav->DataLength());

  return 0;
}
