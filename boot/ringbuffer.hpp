#pragma once
#include "arena.hpp"
#include "terminal.hpp"
#include "types.hpp"

class RingBuffer {
 private:
  u64* buffer_;
  u64 capacity_;
  u64 size_;
  u64 head_;
  u64 tail_;

 public:
  explicit RingBuffer(u64 cap) : capacity_(cap), size_(0), head_(0), tail_(0) {
    buffer_ = (u64*)arena::arena_kmalloc(capacity_ * sizeof(u64));
  }

  u64 getHead() { return head_; }
  u64 getTail() { return tail_; }
  u64 getSize() { return size_; }

  void push(u64 value) {
    buffer_[tail_] = value;
    tail_ = (tail_ + 1) % capacity_;
    if (size_ == capacity_) {
      head_ = (head_ + 1) % capacity_;  // full: drop the oldest entry
    } else {
      size_++;
    }
  }

  u64 pop() {
    u64 value = buffer_[head_];
    head_ = (head_ + 1) % capacity_;
    size_--;
    return value;
  }

  // Prints oldest -> newest, space separated, always ending in its own
  // newline so it never runs into whatever gets printed next.
  void print() {
    terminal::write_string("ringbuffer[");
    terminal::write_uint(size_);
    terminal::write_string("]: ");
    u64 idx = head_;
    for (u64 i = 0; i < size_; i++) {
      terminal::write_uint(buffer_[idx]);
      terminal::putchar(' ');
      idx = (idx + 1) % capacity_;
    }
    terminal::putchar('\n');
  }
};
