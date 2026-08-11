# Boot architecture

How this kernel boots under QEMU, and why each module is split into
`.hpp` + `.cpp` + `.s`.

## 1. What QEMU actually is

QEMU emulates an entire late-90s/2000s PC: a real i686 CPU (with real
privilege rings and a real interrupt system), a BIOS, a couple of chips
(8259 PIC, PIT timer, keyboard controller), and memory-mapped VGA text
output. Your kernel doesn't know it's virtual — it pokes the exact same
CPU instructions, I/O ports, and memory addresses a real 1990s PC would
expose.

```
+---------------------------- QEMU (virtual PC) -----------------------------+
|                                                                             |
|   CPU (i686)        RAM                8259 PIC        VGA text buffer    |
|   real -> protected  kernel.bin loaded  (interrupt       0xB8000          |
|   mode, rings 0-3    at 1 MiB           controller,      memory-mapped    |
|                       (per linker.ld)    emulated chip)   character cells |
|                                                                             |
+-----------------------------------------------------------------------------+
```

## 2. Boot sequence

```
[QEMU powers on]
      |
      v
[SeaBIOS]              <- QEMU's stand-in BIOS
      |
      v
[GRUB]                  reads kernel.bin's multiboot header (boot.s .multiboot
      |                 section, checked via MAGIC/FLAGS/CHECKSUM)
      |                 switches the CPU into 32-bit protected mode
      |                 builds its OWN throwaway GDT
      |                 loads kernel.bin at 1 MiB, jumps to the ELF entry point
      v
[_start   -- boot.s]     sets up a 16 KB stack, calls kernel_main
      v
[kernel_main -- kernel.cpp]
      |
      +--> gdt_install()   replace GRUB's temporary GDT with ours
      +--> idt_install()   build the interrupt table, remap the PIC,
      |                    wire up the isr/irq stubs
      +--> (sti)           enable interrupts -- commented out until the
      |                    TODOs above are filled in for real
      +--> init(vga)       write to VGA text memory
      +--> halt loop (hlt)
```

## 3. Why every module is `.hpp` + `.cpp` + `.s`

Take `gdt` as the example:

```
                    gdt.hpp
              (the CONTRACT: struct layout,
               function signatures both sides
                       agree on)
                   /            \
        included by              included by
                 /                          \
           gdt.cpp                    gdt_flush.s
      (C++ logic: building        (raw asm: the one thing
       the descriptor table --    C++ literally can't express --
       bit math, easy to get      lgdt, then a far jump to
       right and debug in C++)    reload %cs)
                 \                          /
                  \________________________/
                             |
                             v
              linked into one kernel.bin;
           the two sides call each other via
             extern "C" across the boundary
```

- **`.hpp`** — the shared vocabulary. Struct layouts here are dictated by
  the CPU (descriptor byte order, gate format), not by taste, so both the
  C++ and the asm side need to agree on them exactly.
- **`.cpp`** — everything that's just *logic*: packing bits into a table,
  looking up a handler, sending an EOI. No reason to write this in asm —
  C++ is easier to read, easier to get right, and the compiler catches
  mistakes asm won't.
- **`.s`** — only the parts C++ genuinely cannot express: reloading `cs`
  (needs a far jump), the exact register layout the CPU hands you on
  interrupt entry, `iret`. A C++ function has a compiler-chosen
  prologue/epilogue you don't control, so it can't safely be the direct
  target of a CPU interrupt.
- **`extern "C"`** on the shared functions turns off C++ name mangling, so
  the linker can match the plain-text asm label (`gdt_flush:`) to the C++
  declaration (`void gdt_flush(u32)`) — otherwise C++ would rename it to
  something like `_Z9gdt_flushj` and linking would fail.

## 4. Each module, mapped to what QEMU is emulating underneath

| Module | Real hardware it touches | Why it matters |
|---|---|---|
| `gdt_flush.s` / `gdt.cpp` | CPU segmentation unit | Every memory access in protected mode is checked against a segment descriptor — QEMU's virtual CPU enforces this exactly like real silicon |
| `idt_flush.s` / `idt.cpp` | CPU's interrupt-vector-table pointer | Tells the CPU where the 256-entry vector table lives, so it knows where to jump on any interrupt or exception |
| `isr_stubs.s` / `isr.cpp` | CPU exceptions (divide-by-zero, page fault, GPF, ...) | QEMU's CPU raises these the same way real hardware does when something goes wrong |
| `irq_stubs.s` / `irq.cpp` | 8259 PIC (emulated chip) | QEMU emulates this real chip; timer/keyboard signals arrive as IRQ lines that the PIC turns into interrupts |
| `io.hpp` (`inb`/`outb`) | I/O ports | QEMU intercepts these exact port numbers (`0x20`/`0x21` PIC, `0x60`/`0x64` keyboard, ...) and emulates the device sitting behind them |
| VGA write in `init.cpp` | memory address `0xB8000` | QEMU maps this address to its virtual text-mode display — writing a `u16` there *is* drawing a character, same as real VGA hardware |

## 5. Where we are on the privilege ladder

```
 Real mode (16-bit, no memory protection)     <- BIOS/GRUB only, we never touch it
        |
        | GRUB switches the CPU here before jumping to _start
        v
 Protected mode, ring 0 (32-bit, kernel-only) <- WE ARE HERE
        |
        | future work: ring 3 (usermode), a TSS, syscalls
        v
 Protected mode, ring 0 + ring 3
```

## 6. End to end: a timer tick, once the `TODO`s are filled in

```
PIT chip (emulated) ticks
      |
      v
8259 PIC (emulated) raises IRQ0
      |
      v
CPU looks up IDT vector 32 (irq_remap() moved IRQ0 here from 8)
      |
      v
jumps to irq0  (irq_stubs.s)  -- pushes dummy err code + int_no
      |
      v
irq_common_stub               -- pusha, switch to kernel data segment,
      |                          push &regs as the argument
      v
irq_handler(regs)  -- irq.cpp  -- interrupt_handlers[32](regs), then
      |                          send EOI via outb()
      v
irq_common_stub               -- pop everything back
      |
      v
iret                           -- CPU resumes exactly where it was
```
