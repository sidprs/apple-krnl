# Dev Notes

Running log of decisions and explanations from working sessions on this kernel.

## 2026-08-10 — Boot setup, file layout, and modularization plan

### "Booting from ROM..." + "123" on screen — is this OK?

Yes, expected. Running `qemu-system-i386 -kernel kernel.bin` doesn't boot from a
virtual disk — QEMU loads the ELF directly via its built-in Multiboot loader,
which SeaBIOS exposes as a fake "option ROM." SeaBIOS always logs
`Booting from ROM...` when handing off execution that way, regardless of what
the kernel does next — it's just SeaBIOS's boot-device log line, not a kernel
status. The fact that `123` then shows up on screen means `_start` →
`kernel_main` ran and wrote to VGA memory successfully — that's the real
"it worked" signal.

### What was the actual fix (multiboot1 vs multiboot2)

History across commits:

- `691fb9e` — barebones asm, linker, and kernel code (first pass).
- `b285135` — "QEMU -kernel will not work, need a real bootloader, limine":
  the old `boot.asm` used a **Multiboot2** header (magic `0xE85250D6`, NASM
  syntax). QEMU's direct `-kernel` loader only understands **Multiboot1**, so
  the header was invisible to it — would have needed a real bootloader
  (GRUB/Limine) chain-loading from an ISO instead.
- `36fb636` — "proper boot DVD" — built a GRUB-based ISO path as a workaround.
- `91501ab` "[dev] rework" (current `boot/boot.s`) — rewritten in **GNU `as`
  (AT&T) syntax** with a **Multiboot1** header (magic `0x1BADB002`, flags
  `0x3`, checksum). QEMU's `-kernel` flag natively recognizes this format, so
  it can boot `kernel.bin` directly with no GRUB ISO needed for day-to-day
  dev/testing.

The GRUB/ISO path (`make iso`) is still in the Makefile for building a real
bootable image later (real hardware, or a loader that doesn't do direct
Multiboot1 boot).

### File-by-file map (`boot/`)

| File | Role |
|---|---|
| `boot.s` | Real entry point (`_start`). Embeds the Multiboot1 header GRUB/QEMU scan for, sets up a 16KB stack (`stack_bottom`/`stack_top`), calls `kernel_main()`, then halts forever (`cli` + `hlt` loop) if it ever returns. |
| `kernel.cpp` | Kernel code, entered via `extern "C" void kernel_main()`. Currently pokes VGA text-mode memory at `0xB8000` directly (each cell = 1 attribute byte + 1 char byte). |
| `linker.ld` | Linker script. Loads everything at `1M` (standard for Multiboot — below 1MB is reserved/real-mode territory). Forces `.multiboot` to the very front of `.text` via `KEEP(...)` so GRUB/QEMU find the header early in the binary; normal `.text`/`.rodata`/`.data`/`.bss` after. |
| `Makefile` | `boot.s`→`boot.o` (`i686-elf-as`), `kernel.cpp`→`kernel.o` (`i686-elf-g++`, freestanding/no-exceptions/no-rtti), links both via `linker.ld` into `kernel.bin`. `run-kernel` boots directly in QEMU; `iso` packages with GRUB via `grub-mkrescue`. |

### Modularization plan

Freestanding + statically linked, so "modules" = separate `.cpp`/`.hpp`
compilation units linked into one `kernel.bin` — no dynamic loading (that
would be a much later, advanced feature).

Repo already has empty scaffold dirs at the top level meant for this:

```
kernel/     core kernel logic: kernel_main, panic, gdt/idt, scheduler
drivers/    hardware-facing code: vga.cpp, keyboard.cpp, serial.cpp, ata/disk.cpp
lib/        own freestanding "libc": memcpy/memset/strlen, a printf, etc.
include/    headers mirroring the folders above (e.g. include/kernel/scheduler.hpp)
```

Conventions:
- One subsystem per file pair: `drivers/vga.cpp` + `include/drivers/vga.hpp`,
  `kernel/scheduler.cpp` + `include/kernel/scheduler.hpp`. `kernel_main`
  becomes an orchestrator calling `vga::init()`, `gdt::init()`,
  `scheduler::init()`, etc.
- `extern "C"` only at real boundaries (`kernel_main`, interrupt handlers ASM
  calls into) — everything else normal C++ with namespaces instead of C-style
  prefixes.
- Header guards / `#pragma once`; headers stay declaration-only so multiple
  `.cpp` files can share them.
- Makefile should stop hardcoding `boot.o kernel.o` and glob instead:
  `SRCS = $(wildcard kernel/*.cpp drivers/*.cpp lib/*.cpp)` + a pattern rule,
  so new files don't require editing the Makefile.

Gotcha: `.gitignore` has a bare `Makefile` entry, which ignores *any* file
named `Makefile` anywhere in the repo, not just `boot/Makefile`. Existing
tracked `boot/Makefile` is unaffected, but a new Makefile elsewhere (e.g. at
repo root) won't be picked up by `git add` without `-f`.

### What libraries are actually available (freestanding, no libc linked)

**Not available**: `<stdio.h>`, `<stdlib.h>`, `<string.h>` implementations
(malloc, printf, memcpy don't exist), `std::vector`/`std::string`/`std::map`/
any STL needing heap allocation, exceptions (`-fno-exceptions`), RTTI
(`-fno-rtti`).

**Available for free**: pure freestanding headers shipped with the
cross-compiler needing no runtime support — `<stdint.h>`, `<stddef.h>`,
`<stdbool.h>`, `<limits.h>`, C++'s `<type_traits>`, `<utility>`,
`<concepts>`. Just templates/typedefs, no linked code behind them.

**Must write yourself** (what `lib/` is for): `memcpy`, `memset`, `memmove`,
`memcmp` — the compiler emits calls to these for struct copies/array zeroing
even in freestanding code, so missing implementations become link errors.
Also `strlen`/`strcmp`, and a `printf`-style formatter for debugging (write
your own, or vendor a small dependency-free one like mpaland/printf).

**`new`/`delete`**: compiler wants `operator new`/`operator delete` defined
if `new` is used anywhere — implement yourself (placement-new no-op until a
real heap allocator exists).

**`libgcc`**: worth linking (`-lgcc`) — provides compiler-generated helper
routines (e.g. 64-bit division on a 32-bit target) that may be needed even
though it isn't a "library" in the normal sense.

Net effect: filesystem and scheduler work is 100% custom against your own
primitives — no libc underneath to lean on.

### Open items / not yet done
- Root-level working tree still shows deleted `Dockerfile`, `README.md`, old
  `boot.asm`, `kernel.cpp`, `linker.ld`, `kernel.bin`, `iso/` alongside new
  untracked copies under `boot/` — nothing committed yet.
- Makefile still hardcodes `boot.o kernel.o`; not yet updated to glob
  `kernel/`, `drivers/`, `lib/`.
- `kernel/`, `drivers/`, `lib/`, `include/` are currently empty — VGA code
  still lives inline in `boot/kernel.cpp`.
