#pragma once
// Fixed-width types with zero headers. On i686-elf these sizes are
// guaranteed: char=8, short=16, int=32, long long=64.
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i8 = signed char;
using i16 = short;
using i32 = int;
using i64 = long long;
using usize = __SIZE_TYPE__;  // exactly size_t
