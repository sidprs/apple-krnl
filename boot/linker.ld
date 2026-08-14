ENTRY(_start)

SECTIONS
{
  . = 1M;
  
  .text : ALIGN(4)
  {
    KEEP(*(.multiboot))
    *(.text)
  }
  .rodata : ALIGN(4)
  {
    *(.rodata*)
  }
  .data : ALIGN(4)
  {
    *(.data)
  }
  .bss : ALIGN(16)
  {
    *(COMMON)
    *(.bss)
  }
  _kernel_end = .;
}
