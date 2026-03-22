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
export module core:multiset;

import :functions;
import :initializerList;

namespace core {

export template <typename T> class MultiSet {
  struct Node {
    T value;
    Node *next, *prev;
  };
  Node *front, *last;

public:
  ~MultiSet() {
    auto *current = front;
    while (current) {
      auto *next = current->next;
      delete current;
      current = next;
    }
  }

  MultiSet() : front(nullptr), last(nullptr) {}
  MultiSet(initializerList<T> l) : front(nullptr), last(nullptr) {
    for (auto &t : l)
      insert(t);
  }

  void insert(T value) {
    if (front == nullptr) {
      front = last = new Node{.value = value, .next = nullptr, .prev = nullptr};
    }
    auto *newNode = new Node{.value = value, .next = nullptr, .prev = last};
    last->next = newNode;
    last = newNode;
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

  int count(T value) const {
    auto *current = front;
    int result = 0;
    while (current) {
      if (current->value == value)
        ++result;
      current = current->next;
    }
    return result;
  }

  bool contains(Function<bool, T> &filter) const {
    auto *current = front;
    while (current) {
      if (filter(current->value))
        return true;
      current = current->next;
    }
    return false;
  }

  int count(Function<bool, T> &filter) const {
    auto *current = front;
    int result = 0;
    while (current) {
      if (filter(current->value))
        ++result;
      current = current->next;
    }
    return result;
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

  bool remove(T value) {
    auto *current = front;
    while (current) {
      if (current->value == value) {
        if (current->prev) {
          current->prev->next = current->next;
        }
        if (current->next) {
          current->next->prev = current->prev;
        }
        if (current == front) {
          front = current->next;
        }
        if (current == last) {
          last = current->prev;
        }
        delete current;
        return true;
      }
      current = current->next;
    }
    return false;
  }

  void removeAll(T value) {
    auto *current = front;
    while (current) {
      if (current->value == value) {
        if (current->prev) {
          current->prev->next = current->next;
        }
        if (current->next) {
          current->next->prev = current->prev;
        }
        if (current == front) {
          front = current->next;
        }
        if (current == last) {
          last = current->prev;
        }
        delete current;
      }
      current = current->next;
    }
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