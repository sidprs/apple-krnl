#include "types.hpp"
#include "draw.cpp"
extern "C" void kernel_main() {
  volatile u16* vga = (volatile u16*)0xB8000;
  pass(vga);
  while (true) {
    asm volatile("hlt");
  }
}
