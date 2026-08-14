#pragma once

#include "mem.hpp"
#include "tss.hpp"
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

inline GDT_Entry entries[6];
inline GDT_Ptr pointer;
inline TSS_entry tss;

inline void write_tss(int i, u32 kernel_ss, u32 kernel_esp) {
  u32 base = reinterpret_cast<u32>(&tss);
  u32 limit = sizeof(TSS_entry) - 1;
  set_entry(i, base, limit, 0x89, 0x00);

  memset(&tss, 0, sizeof(tss));
  tss.ss0 = kernel_ss;
  tss.esp0 = kernel_esp;
  tss.iomap_base = sizeof(TSS_entry);
}
inline void set_entry(int i, u32 base, u32 limit, u8 access, u8 flags) {
  entries[i].limit_low = limit & 0xFFFF;
  entries[i].base_low = base & 0xFFFF;
  entries[i].base_mid = (base >> 16) & 0xFF;
  entries[i].access = access;
  entries[i].flag_limit = ((limit >> 16) & 0x0F) | (flags & 0xF0);
  entries[i].base_high = (base >> 24) & 0xFF;
}

inline void init() {
  set_entry(0, 0, 0, 0x00, 0x00);                // null descriptor (required)
  set_entry(1, 0, 0xFFFFF, 0x9A, 0xC0);          // kernel code, ring 0
  set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);          // kernel data, ring 0
  set_entry(3, 0, 0xFFFFF, 0xFA, 0xC0);          // user code,   ring 3
  set_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);          // user data,   ring 3
  write_tss(5, 0x10, /* kernek stack top */ 0);  // TSS fill esp0

  pointer.limit = sizeof(entries) - 1;
  pointer.base = reinterpret_cast<u32>(&entries);
  gdt_flush(reinterpret_cast<u32>(&pointer));
  tss_flush(0x28);
}

};  // namespace gdt
