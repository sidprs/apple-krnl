#include "types.hpp"
#include "init.cpp"
extern "C" void kernel_main() {
  volatile u16* vga = (volatile u16*)0xB8000;
  init(vga);
  while (true) {
    asm volatile("hlt");
  }
}


