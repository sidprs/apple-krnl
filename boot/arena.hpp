#pragma once
#include "types.hpp"

namespace arena {

static u8 heap[64 * 1024];
static u8* cursor = heap;

void* arena_kmalloc(usize size, usize align = 8) {
  usize p = ((usize)cursor + (align - 1)) & -(align - 1);
  if (p + size > (usize)heap + sizeof(heap)) return (void*)code;
  cursor = (u8*)(p + size);
  return (void*)p;
}

}  // namespace arena
