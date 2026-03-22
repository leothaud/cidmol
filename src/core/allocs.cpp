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
export module core:allocs;

import :traits;
import :syscalls;
import :builtins;
import :mutex;

namespace core {

export template <u64 allocaSize = 32lu * 1024lu * 1024lu * 1024lu,
                 u64 splitDelta = 1024>
struct Allocator {

#ifdef CORE_THREAD
  Mutex mutex;
#endif

  struct Header {
    Header *next, *prev;
    u64 size;

    Header *getNext() {
      return reinterpret_cast<Header *>(reinterpret_cast<char *>(this) +
                                        offset + size);
    }
  };

  static constexpr u64 offset =
      TraitMax<sizeof(Header), __STDCPP_DEFAULT_NEW_ALIGNMENT__>;

  Header *front;
  Allocator(const Allocator &) = delete;
  Allocator &operator=(const Allocator &) = delete;

  Allocator() {
    front = reinterpret_cast<Header *>(
        syscall<MMAP>(0, allocaSize, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    front->next = front->prev = nullptr;
    front->size = allocaSize - offset;
  }
  u64 totalAlloc = 0;

  Header *findBlock(u64 size) {
    totalAlloc += size;
    auto *current = front;
    while (current) {
      if (current->size >= size)
        return current;
      current = current->next;
    }
    trap();
  }

  void split(Header *block, u64 size) {
    char *mem = reinterpret_cast<char *>(block);
    char *next = mem + offset + size;
    Header *newBlock = reinterpret_cast<Header *>(next);
    newBlock->prev = block;
    newBlock->next = block->next;
    block->next = newBlock;
    newBlock->size = block->size - offset - size;
    block->size = size;
    if (newBlock->next)
      newBlock->next->prev = newBlock;
  }

  void merge(Header *block) {
    if (block->next == block->getNext()) {
      block->size = block->size + offset + block->next->size;
      block->next = block->next->next;
      if (block->next)
        block->next->prev = block;
    }
    if (block->prev && (block->prev->getNext() == block)) {
      block->prev->size = block->prev->size + offset + block->size;
      block->prev->next = block->next;
      if (block->next)
        block->next->prev = block->prev;
    }
  }

  bool needSplit(Header *block, u64 size) {
    return block->size > (splitDelta + offset + size);
  }

  [[gnu::malloc]] void *allocate(u64 size) {

#ifdef CORE_THREAD
    mutex.acquire();
#endif
    Header *block = findBlock(size);
    if (needSplit(block, size)) {
      split(block, size);
    }
    if (block->prev) {
      block->prev->next = block->next;
    }
    if (block->next) {
      block->next->prev = block->prev;
    }
    if (front == block) {
      front = block->next;
    }
    char *mem = reinterpret_cast<char *>(block);

#ifdef CORE_THREAD
    mutex.release();
#endif

    return reinterpret_cast<void *>(mem + offset);
  }

  void reinsert(Header *block) {
    if (!front) {
      front = block;
      block->prev = block->next = nullptr;
      return;
    }

    // assert(front != block, "Double-free.");

    if (reinterpret_cast<u64>(front) > reinterpret_cast<u64>(block)) {
      block->next = front;
      front->prev = block;
      block->prev = nullptr;
      front = block;
      return;
    }

    auto *current = front;
    while (current->next && (reinterpret_cast<u64>(current->next) <
                             reinterpret_cast<u64>(block))) {
      current = current->next;
    }

    // assert(current->next != block, "Double-free.");

    if (current->next) {
      block->next = current->next;
      current->next->prev = block;
      block->prev = current;
      current->next = block;
    } else {
      current->next = block;
      block->prev = current;
      block->next = nullptr;
    }
  }

  void free(void *ptr) {
    if (ptr == nullptr) {
      return;
    }
#ifdef CORE_THREAD
    mutex.acquire();
#endif
    Header *block =
        reinterpret_cast<Header *>(reinterpret_cast<char *>(ptr) - offset);
    reinsert(block);
    merge(block);
#ifdef CORE_THREAD
    mutex.release();
#endif
  }

  static Allocator &get() {
    static Allocator allocator;
    return allocator;
  }
};

export using DefaultAllocator = Allocator<>;

} // namespace core