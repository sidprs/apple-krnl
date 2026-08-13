#pragma once
#include "terminal.hpp"
#include "types.hpp"

inline void init() {
  terminal::initialize();
  terminal::write_string("Hello, pooba World!\n");
  terminal::write_string("Hello, my name is sid!\n");
}
