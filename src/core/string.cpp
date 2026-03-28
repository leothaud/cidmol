/* Copyright 2026 Dylan Leothaud
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
export module core:string;

import :builtins;
import :allocs;
import :cstring;
import :syscalls;
import :traits;

namespace core {

//! @cond INTERNAL

char *fillBuffer(char *buffer, u64 value, u64 &size) {
  if (value >= 10) {
    buffer = fillBuffer(buffer, value / 10, size);
    value = value % 10;
  }
  *buffer = value + '0';
  ++size;
  return buffer + 1;
}

//! @endcond

//! Class representing strings with memory handled by the class for easy usage.
export class String {
  char *buffer;
  u64 size;

public:
  u64 length() const { return size; }
  const char *ptr() const { return buffer; }

  ~String() { delete[] buffer; }

  String() : buffer(new char[1]), size(1) { buffer[0] = 0; }

  template <u64 SIZE>
  String(const char (&value)[SIZE]) : buffer(new char[SIZE]), size(SIZE) {
    memcpy(buffer, value, SIZE);
  }

  String(const char *value) {
    u64 len = strlen(value) + 1;
    buffer = new char[len];
    size = len;
    memcpy(buffer, value, len);
  }

  String(char c) : buffer(new char[2]), size(2) {
    buffer[0] = c;
    buffer[1] = 0;
  }

  String(const String &other) : buffer(new char[other.size]), size(other.size) {
    memcpy(buffer, other.buffer, size);
  }

  String &operator=(const String &other) {
    delete[] buffer;
    buffer = new char[other.size];
    size = other.size;
    memcpy(buffer, other.buffer, size);
    return *this;
  }

  String(String &&other) : buffer(other.buffer), size(other.size) {
    other.buffer = nullptr;
    other.size = 0;
  }

  String &operator=(String &&other) {
    delete[] buffer;
    buffer = other.buffer;
    size = other.size;
    other.buffer = nullptr;
    other.size = 0;
    return *this;
  }

  char operator[](u64 index) const { return buffer[index]; }

  char &operator[](u64 index) { return buffer[index]; }

  String &operator+(String &other) {
    u64 newSize = size + other.size - 1;
    char *newBuffer = new char[newSize];
    memcpy(newBuffer, buffer, size - 1);
    memcpy(newBuffer + size - 1, other.buffer, other.size);
    delete[] buffer;
    buffer = newBuffer;
    size = newSize;
    return *this;
  }

  bool operator==(const String &other) const {
    if (size != other.size)
      return false;
    return streq(buffer, other.buffer, size);
  }
  bool operator!=(const String &other) const { return !(*this == other); }

  String &operator+=(char c) {
    u64 newSize = size + 1;
    char *newBuffer = new char[newSize];
    memcpy(newBuffer, buffer, size - 1);
    newBuffer[size - 1] = c;
    newBuffer[size] = 0;
    delete[] buffer;
    buffer = newBuffer;
    size = newSize;
    return *this;
  }
  String &operator+=(const String &other) {
    u64 newSize = size + other.size - 1;
    char *newBuffer = new char[newSize];
    memcpy(newBuffer, buffer, size - 1);
    memcpy(newBuffer + size - 1, other.buffer, other.size);
    delete[] buffer;
    buffer = newBuffer;
    size = newSize;
    return *this;
  }

  String operator+(const String &other) const {
    String result;
    result.buffer = new char[size + other.size - 1];
    result.size = size + other.size - 1;
    memcpy(result.buffer, buffer, size - 1);
    memcpy(result.buffer + size - 1, other.buffer, other.size);
    return result;
  }

  friend String operator+(const String &lval, const char *rval);
  friend String operator+(const char *lval, const String &rval);

  //! Load a file and puts its content in a String.
  [[nodiscard]] static String fromFile(const String &filename) {
    int fd = open(filename.buffer);
    u64 size = getFileSize(fd);
    String result;
    result.buffer = new char[size + 1];
    result.size = size;
    result.buffer[size] = 0;

    void *mapped = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    memcpy(result.buffer, static_cast<const char *>(mapped), size);
    munmap(mapped, size);
    close(fd);
    return move(result);
  }

  //! Generate a String that reprensent the integer value.
  [[nodiscard]] static String of(u64 value) {
    char buff[21];
    u64 size = 0;
    char *b = fillBuffer(buff, value, size);
    *b = 0;
    const char *ptr = buff;
    return String(ptr);
  }

  class Iterator {
    String *parent;
    u64 index;

  public:
    ~Iterator() = default;
    Iterator(String *parent, u64 index) : parent(parent), index(index) {}

    Iterator(const Iterator &other)
        : parent(other.parent), index(other.index) {}

    char &operator*() { return parent->buffer[index]; }

    char operator*() const { return parent->buffer[index]; }

    bool operator==(const Iterator &other) const {
      return (other.parent == parent) && (other.index == index);
    }

    bool operator!=(const Iterator &other) const { return !(*this == other); }

    Iterator &operator++() {
      if (index != parent->size)
        ++index;
      return *this;
    }
  };

  bool startsWith(String prefix) {
    if (size < prefix.size)
      return false;
    return streq(buffer, prefix.buffer, prefix.size - 1);
  }

public:
  Iterator begin() { return Iterator(this, 0); }
  Iterator end() { return Iterator(this, size); }
  Iterator cbegin() { return Iterator(this, 0); }
  Iterator cend() { return Iterator(this, size); }
};

export String operator+(const String &lval, const char *rval) {
  return lval + String(rval);
}

export String operator+(const char *lval, const String &rval) {
  return String(lval) + rval;
}

} // namespace core