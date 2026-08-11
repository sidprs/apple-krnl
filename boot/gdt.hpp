#pragma once

#include "mem.hpp"
#include "types.hpp"

struct __attribute__((packed)) GDT_Entry {
  u16 limit_low;  //      bits    0 - 15
  u16 base_low;   // base bits    0 - 15
  u8 base_mid;    // base bits   16 - 23
  u8 access;      // P, DPL, S, E, DC, RW, A
  u8 flag_limit;  // limits - (16 - 19), (20 - 23) [G, DB, L, AVL]
  u8 base_high;   // base bits   24 - 31
};

struct __attribute__((packed)) GDT_Ptr {
  u16 limit;
  u32 base;
};

static_assert(sizeof(GDT_Entry) == 8, "GDT_Entry must be 8 bytes");
static_assert(sizeof(GDT_Ptr) == 6, "GDT_Ptr must be 6 bytes");

extern "C" void gdt_flush(u32 gdt_ptr_addr);

namespace gdt {

inline GDT_Entry* entries[5];
inline GDT_Ptr pointer;

inline void set_entry(int i, u32 base, u32 limit, u8 access, u8 flags) {
  entries[i]->limit_low = limit & 0xFFFF;
  entries[i]->base_low = base & 0xFFFF;
  entries[i]->base_mid = (base >> 16) & 0xFF;
}

};  // namespace gdt
