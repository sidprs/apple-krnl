#include "types.hpp"

// draw functions GPU

int main() { 
  
 return 0; 
}

inline usize row = 0;
inline usize col = 0;

static constexpr int WIDTH = 80, HEIGHT = 25;


void init(volatile u16* vga) {

  //  7  6  5  4           3  2  1  0
  // [  background  ]   [ foreground ]
  u16 blank = (0x07 << 8) | ' ';          // grey-on-black space
    for (int i = 0; i < WIDTH * HEIGHT; ++i) 
      vga[i] = blank;
  row = col = 0;
  // [80 x 25]
  // [0   79]
  // [24  79]

  // row = i % 80 
  // col = i / 80
  vga[0] = (0x04 << 8) | 'H';
  vga[1] = (0x04 << 8) | 'I';
}



