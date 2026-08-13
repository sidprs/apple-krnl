#pragma once
#include "types.hpp"

// VGA text-mode driver. Nothing here knows about kernel init or any other
// subsystem -- it only knows how to put characters on the screen.
namespace terminal {

constexpr usize WIDTH = 80;
constexpr usize HEIGHT = 25;

enum Color {
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  MAGENTA = 5,
  BROWN = 6,
  LIGHT_GREY = 7,
  DARK_GREY = 8,
  LIGHT_BLUE = 9,
  LIGHT_GREEN = 10,
  LIGHT_CYAN = 11,
  LIGHT_RED = 12,
  LIGHT_MAGENTA = 13,
  LIGHT_BROWN = 14,
  WHITE = 15,
};

inline u8 entry_color(Color fg, Color bg) { return fg | bg << 4; }
inline u16 entry(unsigned char uc, u8 color) {
  return (u16)uc | (u16)color << 8;
}

inline u16* const buffer = (u16*)0xB8000;
inline usize row = 0;
inline usize column = 0;
inline u8 color = entry_color(BLACK, WHITE);

inline void set_color(u8 c) { color = c; }

// Shift everything up one line instead of wrapping back to row 0 -- wrapping
// is what caused new text to overlap whatever was already on screen.
inline void scroll() {
  for (usize y = 1; y < HEIGHT; y++) {
    for (usize x = 0; x < WIDTH; x++) {
      buffer[(y - 1) * WIDTH + x] = buffer[y * WIDTH + x];
    }
  }
  for (usize x = 0; x < WIDTH; x++) {
    buffer[(HEIGHT - 1) * WIDTH + x] = entry(' ', color);
  }
  row = HEIGHT - 1;
}

inline void put_at(char c, u8 col, usize x, usize y) {
  buffer[y * WIDTH + x] = entry(c, col);
}

inline void newline() {
  column = 0;
  if (++row == HEIGHT) scroll();
}

inline void putchar(char c) {
  if (c == '\n') {
    newline();
    return;
  }
  put_at(c, color, column, row);
  if (++column == WIDTH) newline();
}

inline void write(const char* data, usize size) {
  for (usize i = 0; i < size; i++) putchar(data[i]);
}

inline usize strlen(const char* str) {
  usize len = 0;
  while (str[len]) len++;
  return len;
}

inline void write_string(const char* data) { write(data, strlen(data)); }

inline void write_uint(u64 value) {
  if (value == 0) {
    putchar('0');
    return;
  }
  char digits[20];
  usize n = 0;
  while (value > 0) {
    digits[n++] = '0' + (value % 10);
    value /= 10;
  }
  while (n > 0) putchar(digits[--n]);
}

inline void initialize() {
  row = 0;
  column = 0;
  color = entry_color(BLACK, WHITE);
  for (usize y = 0; y < HEIGHT; y++) {
    for (usize x = 0; x < WIDTH; x++) {
      buffer[y * WIDTH + x] = entry(' ', color);
    }
  }
}

}  // namespace terminal
