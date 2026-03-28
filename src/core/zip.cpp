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
export module core:zip;

import :pair;
import :traits;

namespace core {

//! This is a class that allow iterating over two iterable at once.
//! The precondition is that the two iterable are the same length.
export template <typename Iterable1, typename Iterable2> class Zipped {
  Iterable1 &iterable1;
  Iterable2 &iterable2;

  struct Iterator {
    Iterable1::Iterator it1;
    Iterable2::Iterator it2;

  public:
    auto operator*() const {
      return Pair<Ref<decltype(*it1)>, Ref<decltype(*it2)>>{*it1, *it2};
    }

    bool operator==(const Iterator &other) const {
      return (it1 == other.it1) && (it2 == other.it2);
    }
    bool operator!=(const Iterator &other) const { return !(*this == other); }

    Iterator &operator++() {
      ++it1;
      ++it2;
      return *this;
    }
  };

public:
  Zipped(Iterable1 &iterable1, Iterable2 &iterable2)
      : iterable1(iterable1), iterable2(iterable2) {}
  Iterator begin() const {
    return Iterator{iterable1.begin(), iterable2.begin()};
  }
  Iterator end() const { return Iterator{iterable1.end(), iterable2.end()}; }
};

}; // namespace core