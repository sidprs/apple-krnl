extern "C" void kernel_main() {
    volatile char* vga = (volatile char*)0xB8000;
    vga[0] = 'H';
    vga[1] = 0x07;
}