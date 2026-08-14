#pragma once
#include "types.hpp"

struct __attribute__((packed)) TSS_entry {
  u32 prev_tss;  // unused (hardware task-switching only)
  u32 esp0;      // *** kernel stack pointer loaded on ring3 -> ring0 ***
  u32 ss0;       // *** kernel stack segment (your kernel data selector) ***
  u32 esp1, ss1, esp2, ss2;
  u32 cr3, eip, eflags;
  u32 eax, ecx, edx, ebx, esp, ebp, esi, edi;
  u32 es, cs, ss, ds, fs, gs;
  u32 ldt;
  u16 trap;
  u16 iomap_base;  // set to sizeof(TSS_Entry) = "no I/O bitmap"
};

static_assert(sizeof(TSS_Entry) == 104, "TSS must be 104 bytes");
