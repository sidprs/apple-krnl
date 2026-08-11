#include "types.hpp"

// draw functions GPU

void pass(volatile u16* vga) {
  vga[0] = (0x07 << 8) | '1';
  vga[1] = (0x07 << 8) | '2';
  vga[2] = (0x07 << 8) | '3';
  vga[3] = (0x07 << 8) | '!';
}


int main() { return 0; }
