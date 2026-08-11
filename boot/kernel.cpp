#include "pooba.hpp"
extern "C" void kernel_main() {
  volatile unsigned short* vga = (volatile unsigned short*)0xB8000;

  pass(vga);
  while (true) {
    asm volatile("hlt");
  }
}
