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
export module core:vector;

import :errors;
import :initializerList;
import :traits;
import :builtins;

namespace core {

//! Represent a vector of values.\n
//! Values are stored in a contiguous buffer.
export template <typename T> class Vector {

  //! Capacity of an empty - just created - vector.
  static constexpr u64 defaultCapacity = 4;

  //! Threshold to limit the capacity growth of the buffer.
  static constexpr u64 capacityThreshold = 1024;

  //! The size of the vector is the number of element stored in the buffer.
  u64 size;
  //! The capacity of the vector is the maximum number of elment that can be
  //! stored in the buffer.
  u64 capacity;
  //! The buffer storing the values in the vector.
  T *buffer;

  //! Increase the size of the buffer.\n
  //! If the current capacity is lower than the threshold `capacityThreshold`,
  //! double the capacity.\n Else, increase the current capacity by
  //! `capacityThreshold`.
  void grow() {
    u64 newCapacity = (capacity < capacityThreshold)
                          ? (2 * capacity)
                          : (capacity + capacityThreshold);
    T *newBuffer = new T[newCapacity];
    for (u64 i = 0; i < size; ++i) {
      newBuffer[i] = move(buffer[i]);
    }
    delete[] buffer;
    buffer = newBuffer;
    capacity = newCapacity;
  }

public:
  ~Vector() { delete[] buffer; }

  Vector()
      : size(0), capacity(defaultCapacity), buffer(new T[defaultCapacity]) {}

  Vector(InitializerList<T> l) : Vector() {
    for (const T &elt : l) {
      pushBack(elt);
    }
  }

  Vector(const Vector &other)
      : size(other.size), capacity(other.capacity),
        buffer(new T[other.capacity]) {
    for (u64 i = 0; i < size; ++i)
      buffer[i] = other.buffer[i];
  }
  Vector &operator=(const Vector &other) {
    delete[] buffer;
    size = other.size;
    capacity = other.capacity;
    buffer = new T[capacity];
    for (u64 i = 0; i < size; ++i)
      buffer[i] = other.buffer[i];
    return *this;
  }

  Vector(Vector &&other)
      : size(other.size), capacity(other.capacity), buffer(other.buffer) {
    other.size = other.capacity = 0;
    other.buffer = nullptr;
  }
  Vector &operator=(Vector &&other) {
    delete[] buffer;
    size = other.size;
    capacity = other.capacity;
    buffer = other.buffer;
    other.size = other.capacity = 0;
    other.buffer = nullptr;
    return *this;
  }

  //! Return the number of elements in the vector.
  u64 length() const { return size; }

  //! Access to an element of the vector at a given position.
  //! @param index: the position of an element.
  //! @pre`index < length()`.
  //! @return A reference to the element at position `index`.
  T &operator[](u64 index) {
    assert(index < size);
    return buffer[index];
  }

  //! Access to an element of the vector at a given position.
  //! @param index: the position of an element.
  //! @pre`index < length()`.
  //! @return A reference to the element at position `index`.
  T &operator[](u64 index) const {
    assert(index < size);
    return buffer[index];
  }

  //! Add the element `value` at the end of the vector.
  //! Call `grow()` if the buffer is too small.
  void pushBack(const T &value) {
    if (size == capacity) {
      grow();
    }
    assert(size < capacity);
    buffer[size++] = value;
  }
  //! Add the element `value` at the end of the vector.
  //! Call `grow()` if the buffer is too small.
  void pushBack(T &&value) {
    if (size == capacity) {
      grow();
    }
    assert(size < capacity);
    buffer[size++] = move(value);
  }

  //! Iterator over the vector buffer.
  class Iterator {
    T *buffer;
    u64 index;

  public:
    ~Iterator() = default;
    Iterator(T *buffer, u64 index) : buffer(buffer), index(index) {}
    Iterator(const Iterator &other)
        : buffer(other.buffer), index(other.index) {}
    T &operator*() const { return buffer[index]; }
    T *operator->() const { return &(buffer[index]); }
    bool operator==(const Iterator &other) const {
      return (buffer == other.buffer) && (index == other.index);
    }
    bool operator!=(const Iterator &other) const { return !(*this == other); }
    Iterator &operator++() {
      ++index;
      return *this;
    }

    Iterator operator+(int index) const {
      Iterator res = *this;
      for (int i = 0; i < index; ++i)
        ++res;
      return res;
    }
  };

  Iterator begin() const { return Iterator(buffer, 0); }
  Iterator end() const { return Iterator(buffer, size); }
};

} // namespace core