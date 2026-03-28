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
export module core:map;

import :functions;
import :initializerList;
import :errors;

namespace core {

//! Map with keys of type `K` and values of type `V`
//! Naive implementation using a list of entry
export template <typename K, typename V> class Map {

  struct Entry {
    const K key;
    V value;
  };
  struct EntryList {
    Entry entry;
    EntryList *next;
  };
  EntryList *front;

public:
  void insert(const K key, V &&value) {
    if (front == nullptr) {
      front = new EntryList{.entry =
                                (Entry){.key = key, .value = core::move(value)},
                            .next = nullptr};
      return;
    }
    auto *current = front;
    while (current) {
      if (current->entry.key == key) {
        current->entry.value = core::move(value);
        return;
      }

      if (current->next == nullptr) {
        current->next = new EntryList{
            .entry = (Entry){.key = key, .value = core::move(value)},
            .next = nullptr};
        return;
      }

      current = current->next;
    }
    unreachable();
  }
  void insert(const K key, const V &value) {
    if (front == nullptr) {
      front = new EntryList{.entry = (Entry){.key = key, .value = value},
                            .next = nullptr};
      return;
    }
    auto *current = front;
    while (current) {
      if (current->entry.key == key) {
        current->entry.value = value;
        return;
      }

      if (current->next == nullptr) {
        current->next = new EntryList{
            .entry = (Entry){.key = key, .value = value}, .next = nullptr};
        return;
      }

      current = current->next;
    }
    unreachable();
  }

  bool contains(const K key) const {
    auto *current = front;
    while (current) {
      if (current->entry.key == key)
        return true;
      current = current->next;
    }
    return false;
  }

  bool exists(Function<bool, const K> filter) {
    auto *current = front;
    while (current) {
      if (filter(current->entry.key))
        return true;
      current = current->next;
    }
    return false;
  }

  V &get(const K key) {
    assert(contains(key));
    auto *current = front;
    while (current) {
      if (current->entry.key == key)
        return current->entry.value;
      current = current->next;
    }
    trap();
  }

  V &get(const K key) const {
    assert(contains(key));
    auto *current = front;
    while (current) {
      if (current->entry.key == key)
        return current->entry.value;
      current = current->next;
    }
    trap();
  }

  V &operator[](K key) { return get(key); }
  V &operator[](K key) const { return get(key); }

  ~Map() {
    auto *current = front;
    while (current) {
      auto *next = current->next;
      delete current;
      current = next;
    }
  }

  Map() : front(nullptr) {}
  Map(InitializerList<Entry> init) : front(nullptr) {
    for (auto entry : init)
      insert(entry.key, entry.value);
  }

  class Iterator {
    EntryList *current;

  public:
    ~Iterator() = default;
    Iterator(EntryList *list) : current(list) {}
    Iterator(const Iterator &other) : current(other.current) {}
    Entry &operator*() const { return current->entry; }
    Entry *operator->() const { return &(current->entry); }
    bool operator==(const Iterator &other) const {
      return current == other.current;
    }
    bool operator!=(const Iterator &other) const { return !(*this == other); }
    Iterator &operator++() {
      if (current) {
        current = current->next;
      }
      return *this;
    }
  };

  Iterator begin() const { return Iterator(front); }
  Iterator end() const { return Iterator(nullptr); }
};

} // namespace core