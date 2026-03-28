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
export module core:set;

import :functions;
import :initializerList;

namespace core {

//! Set that contains unique values of types `T`
//! Naive implementation using a list of elements
export template <typename T> class Set {
  struct Node {
    T value;
    Node *next;
  };
  Node *front;

public:
  ~Set() {
    auto *current = front;
    while (current) {
      auto *next = current->next;
      delete current;
      current = next;
    }
  }

  Set() : front(nullptr) {}
  Set(InitializerList<T> l) : front(nullptr) {
    for (auto &t : l)
      insert(t);
  }

  void insert(T value) {
    if (front == nullptr) {
      front = new Node{.value = value, .next = nullptr};
    }
    auto *current = front;
    while (true) {
      if (current->value == value)
        return;

      if (current->next == nullptr) {
        current->next = new Node{.value = value, .next = nullptr};
        return;
      }

      current = current->next;
    }
    unreachable();
  }

  bool contains(T value) const {
    auto *current = front;
    while (current) {
      if (current->value == value)
        return true;
      current = current->next;
    }
    return false;
  }

  bool exists(Function<bool, T> &filter) const {
    auto *current = front;
    while (current) {
      if (filter(current->value))
        return true;
      current = current->next;
    }
    return false;
  }

  bool forall(Function<bool, T> filter) const {
    auto *current = front;
    while (current) {
      if (!filter(current->value))
        return false;
      current = current->next;
    }
    return true;
  }

  class Iterator {
    Node *current;

  public:
    ~Iterator() = default;
    Iterator(Node *list) : current(list) {}
    Iterator(const Iterator &other) : current(other.current) {}
    T &operator*() const { return current->value; }
    T *operator->() const { return &(current->value); }
    bool operator==(const Iterator &other) const {
      return current == other.current;
    }
    bool operator!=(const Iterator &other) const { return !(*this == other); }
    Iterator &operator++() {
      if (current)
        current = current->next;
      return *this;
    }
  };

  Iterator begin() const { return Iterator(front); }
  Iterator end() const { return Iterator(nullptr); }
};

} // namespace core
