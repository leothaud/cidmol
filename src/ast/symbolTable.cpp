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
export module ast:symbolTable;

import core;
import :fwd;

namespace ast::symbolTable {

export struct Entry {
  core::String name;
  core::UniquePtr<ast::Type> type;
};

export struct Level {
  core::SharedPtr<Level> parent;
  core::Vector<core::SharedPtr<Entry>> entries;

  Level(core::SharedPtr<Level> parent);

  core::SharedPtr<Entry> lookupInLevel(core::String name);
  core::SharedPtr<Entry> lookup(core::String name);
  core::LogicalResult<core::SharedPtr<Entry>>
  insert(core::String name, core::UniquePtr<Type> type);
};

export struct Table {

  core::SharedPtr<Level> current;
  core::Map<core::String, core::UniquePtr<Type>> aliases;
  core::Map<core::String, core::UniquePtr<StructDeclaration>> structs;
  core::Map<core::String, core::UniquePtr<UnionDeclaration>> unions;

  Table();

  core::SharedPtr<Entry> lookupInLevel(core::String name);
  core::SharedPtr<Entry> lookup(core::String name);
  core::LogicalResult<core::SharedPtr<Entry>>
  insert(core::String name, core::UniquePtr<Type> type);
  void initialize();
  void finalize();
  void insertAlias(core::String name, core::UniquePtr<Type> type);
  core::LogicalResult<core::UniquePtr<Type>> getAlias(core::String name);
};

} // namespace ast::symbolTable