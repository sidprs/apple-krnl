extern "C" void kernel_main() {
  // VGA text buffer at 0xB8000
  volatile char* vga = reinterpret_cast<volatile char*>(0xB8000);
  const char* msg = "Hello from the kernel!";
  for (int i = 0; msg[i]; ++i) {
    vga[i * 2] = msg[i];
    vga[i * 2 + 1] = 0x0F;  // white on black
  }
  while (true) {
  }
}
