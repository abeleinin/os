#include <stdio.h>
#include <signal.h>

#define SEG_MASK 0x3000
#define SEG_SHIFT 12
#define OFFSET_MASK 0xFFF

// Segment sizes:
// Code 2K, Heap 3K, Stack 2K
int Bounds[3] = {2048, 3072, 2048};

// Base (Physical address):
// Code 32K, Heap 34K, Stack 28K
int Base[3] = {32768, 34816, 28672};

int main() {
  // get top 2 bits of 14-bit virtual address
  // virtual address is just 4200 in decimal
  unsigned int virtualAddress = 0b01000001101000;
  int segment = (virtualAddress & SEG_MASK) >> SEG_SHIFT;

  // get offset
  int offset = virtualAddress & OFFSET_MASK;

  printf("segment=%d\n", segment);
  printf("offset=%d\n", offset);

  // hypothetically...
  if (offset >= Bounds[segment]) {
    printf("RaiseException(PROTECTION_FAULT)\n");
    raise(SIGSEGV);
  } else {
    int physicalAddress = Base[segment] + offset;
    printf("register=AccessMemory(PhysicalAddress=%d)\n", physicalAddress);
  }

  return 0;
}
