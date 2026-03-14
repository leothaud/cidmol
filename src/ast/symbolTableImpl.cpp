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

import :ast;
import :symbolTable;

namespace ast::symbolTable {

Level::Level(core::SharedPtr<Level> parent) : parent(parent) {}

core::SharedPtr<Entry> Level::lookupInLevel(core::String name) {
  for (auto &entry : entries) {
    if (entry->name == name)
      return entry;
  }
  return nullptr;
}

core::SharedPtr<Entry> Level::lookup(core::String name) {
  auto inLevel = lookupInLevel(name);
  if (inLevel)
    return inLevel;
  if (parent)
    return parent->lookup(name);
  return nullptr;
}

core::LogicalResult<core::SharedPtr<Entry>>
Level::insert(core::String name, core::UniquePtr<Type> type) {
  if (lookupInLevel(name)) {
    return core::LogicalResult<core::SharedPtr<Entry>>::failure(
        "Variable " + name + " already exists.");
  }
  auto result = core::SharedPtr<Entry>::create(name, core::move(type));
  entries.pushBack(result);
  return result;
}

Table::Table() : current(core::SharedPtr<Level>::create(nullptr)) {}

core::SharedPtr<Entry> Table::lookupInLevel(core::String name) {
  return current->lookupInLevel(name);
}

core::SharedPtr<Entry> Table::lookup(core::String name) {
  return current->lookup(name);
}

core::LogicalResult<core::SharedPtr<Entry>>
Table::insert(core::String name, core::UniquePtr<Type> type) {
  return current->insert(name, core::move(type));
}

void Table::initialize() { current = core::SharedPtr<Level>::create(current); }

void Table::finalize() { current = current->parent; }

void Table::insertAlias(core::String name, core::UniquePtr<Type> type) {
  aliases.insert(name, core::move(type));
}

core::LogicalResult<core::UniquePtr<Type>> Table::getAlias(core::String name) {
  if (aliases.contains(name)) {
    return aliases[name]->clone();
  }
  return core::LogicalResult<core::UniquePtr<Type>>::failure("Unknown type " +
                                                             name + ".");
}

} // namespace ast::symbolTable