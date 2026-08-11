#include "types.hpp"

// draw functions GPU
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

static inline u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

static inline u16 vga_entry(unsigned char uc, u8 color) {
  return (u16)uc | (u16)color << 8;
}

usize strlen(const char* str) {
  usize len = 0;
  while (str[len]) len++;
  return len;
}

usize terminal_row;
usize terminal_column;
u8 terminal_color;
u16* terminal_buffer = (u16*)VGA_MEMORY;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);

  for (usize y = 0; y < VGA_HEIGHT; y++) {
    for (usize x = 0; x < VGA_WIDTH; x++) {
      const usize index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setcolor(u8 color) { terminal_color = color; }

void terminal_putentryat(char c, u8 color, usize x, usize y) {
  const usize index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) terminal_row = 0;
  }
}

void terminal_write(const char* data, usize size) {
  for (usize i = 0; i < size; i++) terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
  terminal_write(data, strlen(data));
}

void init(volatile u16* vga) {
  //  7  6  5  4           3  2  1  0
  // [  background  ]   [ foreground ]
  terminal_initialize();
  terminal_writestring("Hello, kernel World!\n");
}
