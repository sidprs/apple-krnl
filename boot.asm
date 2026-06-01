; boot.asm - kernel entry point
extern kernel_main
section .multiboot
align 8
  dd 0xE85250D6         ; magic
  dd 0                  ; arch i386 protected mode
  dd 24                 ; header length 
  dd -(0xE85250D6 + 0 + 24) ; checksum

section .text
global _start
_start:
    call kernel_main
    hlt
