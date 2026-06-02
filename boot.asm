; ; boot.asm - kernel entry point


; ; Offset	Type	Field Name	
; ; 0	      u32	   magic	
; ; 4	      u32	   architecture	
; ; 8	      u32	   header_length	
; ; 12	    u32	   checksum	
; ; 16-XX		       tags	

; section .multiboot
; align 8

; header_start:
;     dd 0xE85250D6          ; magic
;     dd 0                   ; architecture (i386 = 0)
;     dd header_end - header_start

;     ; ---- REQUIRED END TAG ----
;     dw 0                   ; type = end tag
;     dw 0                   ; flags
;     dd 8                   ; size

; header_end:



; -----------------------------
; Kernel entry point
; -----------------------------
[BITS 32]               ; GRUB starts us in 32-bit mode!
global _start
extern kernel_main      ; Tell NASM kernel_main is in another file

; multiboot2 header here is purely a binary signature embedded in your kernel so GRUB can recognize it
section .multiboot
align 8
header_start:
    dd 0xe85250d6                
    dd 0                         
    dd header_end - header_start 
    dd -(0xE85250D6 + 0 + (header_end - header_start))
    dw 0    
    dw 0    
    dd 8    
header_end:

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
_start:
    mov esp, stack_top    ; Use 32-bit ESP
    
    ; NOTE: If kernel_main is 64-bit, this call will still crash 
    ; until you implement the "Long Mode" switch. 
    ; For now, keep it 32-bit to see if it boots.
    call kernel_main      
    
    cli
.hang:
    hlt
    jmp .hang
