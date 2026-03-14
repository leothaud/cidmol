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
module ast;

import core;
import :ast;
import :symbolTable;

namespace ast {

Node::Node(u64 line_start, u64 column_start, u64 line_end, u64 column_end)
    : line_start(line_start), column_start(column_start), line_end(line_end),
      column_end(column_end) {}

Node::Node() : line_start(0), column_start(0), line_end(0), column_end(0) {}

u64 Node::getLineStart() const { return line_start; }
u64 Node::getColumnStart() const { return column_start; }
u64 Node::getLineEnd() const { return line_end; }
u64 Node::getColumnEnd() const { return column_end; }

u64 ProgramItem::getLineStart() const {
  return this->visit([](auto &&elt) { return elt.getLineStart(); });
}
u64 ProgramItem::getColumnStart() const {
  return this->visit([](auto &&elt) { return elt.getColumnStart(); });
}
u64 ProgramItem::getLineEnd() const {
  return this->visit([](auto &&elt) { return elt.getLineEnd(); });
}
u64 ProgramItem::getColumnEnd() const {
  return this->visit([](auto &&elt) { return elt.getColumnEnd(); });
}

u64 Type::getLineStart() const {
  return this->visit([](auto &&elt) { return elt.getLineStart(); });
}
u64 Type::getColumnStart() const {
  return this->visit([](auto &&elt) { return elt.getColumnStart(); });
}
u64 Type::getLineEnd() const {
  return this->visit([](auto &&elt) { return elt.getLineEnd(); });
}
u64 Type::getColumnEnd() const {
  return this->visit([](auto &&elt) { return elt.getColumnEnd(); });
}

u64 Statement::getLineStart() const {
  return this->visit([](auto &&elt) { return elt.getLineStart(); });
}
u64 Statement::getColumnStart() const {
  return this->visit([](auto &&elt) { return elt.getColumnStart(); });
}
u64 Statement::getLineEnd() const {
  return this->visit([](auto &&elt) { return elt.getLineEnd(); });
}
u64 Statement::getColumnEnd() const {
  return this->visit([](auto &&elt) { return elt.getColumnEnd(); });
}

u64 SwitchItem::getLineStart() const {
  return this->visit([](auto &&elt) { return elt.getLineStart(); });
}
u64 SwitchItem::getColumnStart() const {
  return this->visit([](auto &&elt) { return elt.getColumnStart(); });
}
u64 SwitchItem::getLineEnd() const {
  return this->visit([](auto &&elt) { return elt.getLineEnd(); });
}
u64 SwitchItem::getColumnEnd() const {
  return this->visit([](auto &&elt) { return elt.getColumnEnd(); });
}

u64 Expression::getLineStart() const {
  return this->visit([](auto &&elt) { return elt.getLineStart(); });
}
u64 Expression::getColumnStart() const {
  return this->visit([](auto &&elt) { return elt.getColumnStart(); });
}
u64 Expression::getLineEnd() const {
  return this->visit([](auto &&elt) { return elt.getLineEnd(); });
}
u64 Expression::getColumnEnd() const {
  return this->visit([](auto &&elt) { return elt.getColumnEnd(); });
}

} // namespace ast