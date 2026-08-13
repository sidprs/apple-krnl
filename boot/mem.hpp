// memcpy, mempool

#pragma once
#include "types.hpp"
// memcpy notes

extern "C" void* memcpy(void* dst, const void* src, usize len) {
  auto* d = (u8*)dst;
  auto* s = (const u8*)src;
  for (usize i = 0; i < len; ++i) d[i] = s[i];
  return dst;
}

// memcpy notes
extern "C" void* memset(void* dst, int c, usize n) {
  auto* d = (u8*)dst;
  for (usize i = 0; i < n; ++i) d[i] = (u8)c;
  return dst;
}

namespace arena {

static u8 heap[64 * 1024];
static u8* cursor = heap;

void* arena_kmalloc(usize size, usize align = 8) {
  usize p = ((usize)cursor + (align - 1)) & ~(align - 1);
  if (p + size > (usize)heap + sizeof(heap)) return (void*)code;
  cursor = (u8*)(p + size);
  return (void*)p;
}

inline void reset() {
  memset(heap, 0, cursor - heap);
  cursor = heap;
}

}  // namespace arena

// arena allocater
