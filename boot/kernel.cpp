#include "init.cpp"
#include "types.hpp"
extern "C" void kernel_main() {
  volatile u16* vga = (volatile u16*)0xB8000;
  init(vga);

  // forever loop halting once
  while (true) {
    asm volatile("hlt");
  }
}
