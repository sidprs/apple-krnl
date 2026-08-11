// memcpy, mempool
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

// arena allocater
