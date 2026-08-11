# i686-elf-as boot.s -o boot.o
.set MAGIC, 0x1BADB002
.set FLAGS, 0x00000003

.set CHECKSUM, -(MAGIC + FLAGS)


.section .multiboot # 0x1000 % 4 = 0 (if it was starting at )
.align 4 # aligned to 4b
.long MAGIC
.long FLAGS
.long CHECKSUM


.section .bss
.align 16
stack_bottom: .skip 16384
#16384 bytes is 16KB 
stack_top:

.section .text
.global _start
_start:
  mov $stack_top, %esp
  call kernel_main
  cli

# diables interupts and halts forever in kernel_main ever returns

1: hlt
  jmp 1b
