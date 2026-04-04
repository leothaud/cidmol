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
export module core:stream;

import :builtins;
import :string;
import :ioUtils;

namespace core {

//! Stream wrapper around a file descriptor.
export class FdStream {
protected:
  int fd;

public:
  constexpr explicit FdStream(int fd) : fd(fd) {}

  FdStream &operator<<(const char *str) {
    write(fd, str);
    return *this;
  }
  FdStream &operator<<(i8 i);
  FdStream &operator<<(i16 i);
  FdStream &operator<<(i32 i);
  FdStream &operator<<(i64 i);

  FdStream &operator<<(u8 i);
  FdStream &operator<<(u16 i);
  FdStream &operator<<(u32 i);
  FdStream &operator<<(u64 i);

  FdStream &operator<<(Hex x);
  template <typename... Ts> FdStream &operator<<(Color<Ts...> x);

  FdStream &operator<<(String str) {
    write(fd, str.ptr(), str.length() - 1);
    return *this;
  }
  FdStream &operator<<(char c) {
    write(fd, &c, 1);
    return *this;
  }

  FdStream &operator>>(i8 &out);
  FdStream &operator>>(i16 &out);
  FdStream &operator>>(i32 &out);
  FdStream &operator>>(i64 &out);
  FdStream &operator>>(u8 &out);
  FdStream &operator>>(u16 &out);
  FdStream &operator>>(u32 &out);
  FdStream &operator>>(u64 &out);
  FdStream &operator>>(char &out) {
    read(fd, &out, 1);
    return *this;
  }
};

//! @cond INTERNAL

template <bool SIGNED = true> FdStream &printInt(FdStream &stream, u64 value);
template <> FdStream &printInt<false>(FdStream &stream, u64 value);
template <> FdStream &printInt<true>(FdStream &stream, u64 value);

char *fillBuffer(char *buffer, u64 v) {
  if (v >= 10) {
    buffer = fillBuffer(buffer, v / 10);
    v = v % 10;
  }
  *buffer = v + '0';
  return buffer + 1;
}

template <> FdStream &printInt<true>(FdStream &stream, u64 value) {
  i64 v = value;
  if (v < 0) {
    stream << '-';
    v = -v;
  }
  return printInt<false>(stream, v);
}

template <> FdStream &printInt<false>(FdStream &stream, u64 value) {
  char buff[21];
  char *b = buff;
  b = fillBuffer(b, value);
  *b = 0;
  stream << buff;
  return stream;
}

i64 readInt(int fd) {
  char buff;
  u64 rd = read(fd, &buff, 1);
  if (rd == 0)
    return 0;
  bool isSigned = false;
  if (buff == '-') {
    isSigned = true;
    rd = read(fd, &buff, 1);
    if (rd == 0)
      return 0;
  }
  i64 value = 0;
  bool finished = false;
  do {
    switch (buff) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      value = 10 * value + (buff - '0');
      rd = read(fd, &buff, 1);
      if (rd == 0)
        finished = true;
      break;
    }
    default: {
      finished = true;
    }
    }
  } while (!finished);
  return isSigned ? -value : value;
}

//! @endcond

FdStream &FdStream::operator<<(i8 i) { return printInt<true>(*this, (i64)i); }
FdStream &FdStream::operator<<(i16 i) { return printInt<true>(*this, (i64)i); }
FdStream &FdStream::operator<<(i32 i) { return printInt<true>(*this, (i64)i); }
FdStream &FdStream::operator<<(i64 i) { return printInt<true>(*this, (i64)i); }

FdStream &FdStream::operator<<(u8 i) { return printInt<false>(*this, (i64)i); }
FdStream &FdStream::operator<<(u16 i) { return printInt<false>(*this, (i64)i); }
FdStream &FdStream::operator<<(u32 i) { return printInt<false>(*this, (i64)i); }
FdStream &FdStream::operator<<(u64 i) { return printInt<false>(*this, (i64)i); }

FdStream &FdStream::operator<<(Hex x) {
  *this << "0x";
  static char digit[16];
  auto toHex = [] [[gnu::always_inline]] (u64 value) -> char {
    switch (value) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 10:
      return 'a';
    case 11:
      return 'b';
    case 12:
      return 'c';
    case 13:
      return 'd';
    case 14:
      return 'e';
    default:
      return 'f';
    }
  };
  int index = 15;
  u64 value = x.value;
  while (value > 0) {
    u64 next = value & 0xFllu;
    digit[index--] = toHex(next);
    value >>= 4;
  }
  if (index == 15) {
    *this << '0';
  } else {
    for (int i = index; i < 16; ++i) {
      *this << digit[i];
    }
  }
  return *this;
}

template <typename H, typename... T>
  requires(sizeof...(T) > 0)
FdStream *ColorTuple<H, T...>::printElements(FdStream *stream) {
  *stream << head;
  return tail.printElements(stream);
}

template <typename T> FdStream *ColorTuple<T>::printElements(FdStream *stream) {
  *stream << elt;
  return stream;
}

template <typename... Ts> FdStream &FdStream::operator<<(Color<Ts...> x) {
  *this << "\033[";
  if (x.options.backgroundColor != color::background::DEFAULT) {
    *this << static_cast<int>(x.options.backgroundColor);

    if (x.options.textColor != color::text::DEFAULT) {
      *this << ";";
    }
  }
  if (x.options.textColor != color::text::DEFAULT) {
    *this << static_cast<int>(x.options.textColor);
  }
  *this << "m";
  x.values.printElements(this);
  *this << "\033[m";
  return *this;
}

FdStream &FdStream::operator>>(i8 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(i16 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(i32 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(i64 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(u8 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(u16 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(u32 &out) {
  out = readInt(this->fd);
  return *this;
}

FdStream &FdStream::operator>>(u64 &out) {
  out = readInt(this->fd);
  return *this;
}

//! Default IOs stream.
export FdStream sout = FdStream(stdout), serr = FdStream(stderr),
                sin = FdStream(stdin);

//! Stream wrapper around a file with auto openning and closing.
export class FileStream : public FdStream {
public:
  explicit FileStream(const char *filename) : FdStream(open(filename)) {}
  explicit FileStream(const String filename) : FileStream(filename.ptr()) {}
  ~FileStream() {
    if (fd != stdout && fd != stdin && fd != stderr)
      close(fd);
  }

  FileStream(const FileStream &) = delete;
  FileStream &operator=(const FileStream &) = delete;
  FileStream(FileStream &&other) : FdStream(other.fd) { other.fd = stdout; }
  FileStream &operator=(FileStream &&other) {
    if (fd != stdout && fd != stdin && fd != stderr)
      close(fd);
    fd = other.fd;
    other.fd = stdout;
    return *this;
  }
};

//! Stream wrapper that constructs a String when written into.
export class StringStream {
  core::String buffer;

public:
  StringStream() : buffer{} {}
  core::String toString() { return buffer; }

  StringStream &operator<<(const char *str) {
    buffer += str;
    return *this;
  }
  StringStream &operator<<(i8 i) {
    if (i < 0) {
      buffer += "-";
      i = -i;
    }
    buffer += core::String::of(i);
    return *this;
  }
  StringStream &operator<<(i16 i) {
    if (i < 0) {
      buffer += "-";
      i = -i;
    }
    buffer += core::String::of(i);
    return *this;
  }
  StringStream &operator<<(i32 i) {
    if (i < 0) {
      buffer += "-";
      i = -i;
    }
    buffer += core::String::of(i);
    return *this;
  }
  StringStream &operator<<(i64 i) {
    if (i < 0) {
      buffer += "-";
      i = -i;
    }
    buffer += core::String::of(i);
    return *this;
  }

  StringStream &operator<<(u8 i) {
    buffer += core::String::of(i);
    return *this;
  }

  StringStream &operator<<(u16 i) {
    buffer += core::String::of(i);
    return *this;
  }

  StringStream &operator<<(u32 i) {
    buffer += core::String::of(i);
    return *this;
  }

  StringStream &operator<<(u64 i) {
    buffer += core::String::of(i);
    return *this;
  }

  StringStream &operator<<(String str) {
    buffer += str;
    return *this;
  }

  StringStream &operator<<(char c) {
    buffer += c;
    return *this;
  }
};

} // namespace core