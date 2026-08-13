#include "init.hpp"
#include "ringbuffer.hpp"
#include "types.hpp"

extern "C" void kernel_main() {
  init();

  RingBuffer r(255);
  r.push(1);
  r.push(2);
  r.push(3);
  r.print();

  // forever loop halting once
  while (true) {
    asm volatile("hlt");
  }
}
