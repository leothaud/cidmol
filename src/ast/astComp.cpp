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
import core;

namespace {

template <typename T>
core::LogicalResult<void> comparisonError(core::String message, const T &from,
                                          const T &to) {
  return core::LogicalResult<void>::failure(
      "Comparison error at " + core::String::of(from.getLineStart()) + ":" +
      core::String::of(from.getColumnStart()) + "/" +
      core::String::of(to.getLineStart()) + ":" +
      core::String::of(to.getColumnStart()) + ":\n" + message + "\n");
}

core::LogicalResult<void> success() {
  return core::LogicalResult<void>::success();
}

template <core::StringLiteral NAME, core::StringLiteral ITEM_NAME, typename T,
          typename BASE>
core::LogicalResult<void>
compareVector(const BASE &parentFrom, const BASE &parentTo,
              const core::Vector<core::UniquePtr<T>> &from,
              const core::Vector<core::UniquePtr<T>> &to) {
  if (from.length() != to.length()) {
    return comparisonError(static_cast<core::String>(NAME) +
                               " has different numbers of " +
                               static_cast<core::String>(ITEM_NAME) + ". " +
                               core::String::of(from.length()) + " vs " +
                               core::String::of(to.length()) + ".",
                           parentFrom, parentTo);
  }
  for (unsigned idx = 0; idx < from.length(); ++idx) {
    auto cmp = from[idx]->isEqual(*to[idx]);
    if (cmp.failed())
      return cmp;
  }
  return success();
}

template <typename T>
core::LogicalResult<void> binexprIsEqual(const T &from, const T &to) {
  auto cmp = from.lval->isEqual(*to.lval);
  if (cmp.failed())
    return cmp;
  return from.rval->isEqual(*to.rval);
}

} // namespace

namespace ast {

core::LogicalResult<void> Program::isEqual(const Program &other) const {
  return compareVector<"Programs ", "items">(*this, other, items, other.items);
}

core::LogicalResult<void>
FunctionDeclaration::isEqual(const FunctionDeclaration &other) const {
  auto cmp = returnType->isEqual(*other.returnType);
  if (cmp.failed())
    return cmp;
  if (name != other.name) {
    return comparisonError("Function names are different. `" + name + "` vs `" +
                               other.name + "`.",
                           *this, other);
  }
  cmp =
      compareVector<"Functions ", "arguments">(*this, other, args, other.args);
  if (cmp.failed())
    return cmp;
  return body->isEqual(*other.body);
}

core::LogicalResult<void> Argument::isEqual(const Argument &other) const {
  auto cmp = type->isEqual(*other.type);
  if (cmp.failed())
    return cmp;
  if (name != other.name) {
    return comparisonError("Arguments have differents name `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void>
StructDeclaration::isEqual(const StructDeclaration &other) const {
  if (name != other.name) {
    return comparisonError("Struct declarations have differents name `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  auto cmp = compareVector<"Struct declarations", "fields">(
      *this, other, fields, other.fields);
  if (cmp.failed())
    return cmp;
  cmp = compareVector<"Struct declarations", "operators">(*this, other, ops,
                                                          other.ops);
  if (cmp.failed())
    return cmp;
  cmp = compareVector<"Struct declarations", "methods">(*this, other, methods,
                                                        other.methods);
  if (cmp.failed())
    return cmp;
  return compareVector<"Struct declarations", "cast declarations">(
      *this, other, casts, other.casts);
}

core::LogicalResult<void>
UnionDeclaration::isEqual(const UnionDeclaration &other) const {
  if (name != other.name) {
    return comparisonError("Union declarations have different names `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  return compareVector<"Union declarations", "fields">(*this, other, fields,
                                                       other.fields);
}

core::LogicalResult<void>
CastDeclaration::isEqual(const CastDeclaration &other) const {
  auto cmp = type->isEqual(*other.type);
  if (cmp.failed())
    return cmp;
  return body->isEqual(*other.body);
}

core::LogicalResult<void>
OperationDeclaration::isEqual(const OperationDeclaration &other) const {
  if (opType != other.opType) {
    return comparisonError(
        "Operator declaration declare differents operators. " +
            to_string(opType) + " vs " + to_string(other.opType) + ".",
        *this, other);
  }
  return body->isEqual(*other.body);
}

core::LogicalResult<void>
VariableDeclaration::isEqual(const VariableDeclaration &other) const {
  auto cmp = varType->isEqual(*other.varType);
  if (cmp.failed())
    return cmp;
  return compareVector<"Variable declarations", "declarators">(
      *this, other, decls, other.decls);
}

core::LogicalResult<void> Declarator::isEqual(const Declarator &other) const {
  if (name != other.name) {
    return comparisonError("Declarators have different names. `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  if ((value == nullptr) || (other.value == nullptr)) {
    if ((value == nullptr) && (other.value == nullptr))
      return success();
    return comparisonError("Only one of the declarators is initialized.", *this,
                           other);
  } else {
    return value->isEqual(*other.value);
  }
}

core::LogicalResult<void>
CompoundStmt::isEqual(const CompoundStmt &other) const {
  return compareVector<"Compound statements", "items">(*this, other, items,
                                                       other.items);
}

core::LogicalResult<void> WhileStmt::isEqual(const WhileStmt &other) const {
  auto cmp = cond->isEqual(*other.cond);
  if (cmp.failed())
    return cmp;
  return body->isEqual(*other.body);
}

core::LogicalResult<void> DoWhileStmt::isEqual(const DoWhileStmt &other) const {
  auto cmp = cond->isEqual(*other.cond);
  if (cmp.failed())
    return cmp;
  return body->isEqual(*other.body);
}

core::LogicalResult<void> ForStmt::isEqual(const ForStmt &other) const {
  if ((init != nullptr) || (other.init != nullptr)) {
    if ((init == nullptr) || (other.init == nullptr)) {
      return comparisonError("Only one for loop have an initialization.", *this,
                             other);
    }
    auto cmp = init->isEqual(*other.init);
    if (cmp.failed())
      return cmp;
  }
  if ((cond != nullptr) || (other.cond != nullptr)) {
    if ((cond == nullptr) || (other.init == nullptr)) {
      return comparisonError("Only one for loop have a condition.", *this,
                             other);
    }
    auto cmp = cond->isEqual(*other.cond);
    if (cmp.failed())
      return cmp;
  }
  if ((step != nullptr) || (other.step != nullptr)) {
    if ((step == nullptr) || (other.step == nullptr)) {
      return comparisonError("Only one for loop have a step expression.", *this,
                             other);
    }
    auto cmp = step->isEqual(*other.step);
    if (cmp.failed())
      return cmp;
  }
  return body->isEqual(*other.body);
}

core::LogicalResult<void> IfStmt::isEqual(const IfStmt &other) const {
  auto cmp = cond->isEqual(*other.cond);
  if (cmp.failed())
    return cmp;
  cmp = thenPart->isEqual(*other.thenPart);
  if (cmp.failed())
    return cmp;
  if ((elsePart != nullptr) || (other.elsePart != nullptr)) {
    if ((elsePart == nullptr) || (other.elsePart == nullptr)) {
      return comparisonError("Only one if statement has an else part.", *this,
                             other);
    }
    return elsePart->isEqual(*other.elsePart);
  }
  return success();
}

core::LogicalResult<void> SwitchStmt::isEqual(const SwitchStmt &other) const {
  auto cmp = cond->isEqual(*other.cond);
  if (cmp.failed())
    return cmp;
  return compareVector<"Switch statements", "items">(*this, other, items,
                                                     other.items);
}

core::LogicalResult<void> SwitchCase::isEqual(const SwitchCase &other) const {
  auto cmp = cond->isEqual(*other.cond);
  if (cmp.failed())
    return cmp;
  return body->isEqual(*other.body);
}

core::LogicalResult<void>
SwitchDefault::isEqual(const SwitchDefault &other) const {
  return body->isEqual(*other.body);
}

core::LogicalResult<void>
ExpressionStmt::isEqual(const ExpressionStmt &other) const {
  return value->isEqual(*other.value);
}

core::LogicalResult<void> NopStmt::isEqual(const NopStmt &) const {
  return success();
}

core::LogicalResult<void> ContinueStmt::isEqual(const ContinueStmt &) const {
  return success();
}

core::LogicalResult<void> BreakStmt::isEqual(const BreakStmt &) const {
  return success();
}

core::LogicalResult<void> ReturnStmt::isEqual(const ReturnStmt &other) const {
  if ((value != nullptr) || (other.value != nullptr)) {
    if ((value == nullptr) || (other.value == nullptr)) {
      return comparisonError("Only one return statement has a value.", *this,
                             other);
    }
    return value->isEqual(*other.value);
  }
  return success();
}

core::LogicalResult<void> PtrType::isEqual(const PtrType &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void> ArrayType::isEqual(const ArrayType &other) const {
  auto cmp = base->isEqual(*other.base);
  if (cmp.failed())
    return cmp;
  return size->isEqual(*other.size);
}

core::LogicalResult<void> FunType::isEqual(const FunType &other) const {
  auto cmp = base->isEqual(*other.base);
  if (cmp.failed())
    return cmp;
  return compareVector<"Function types", "arguments">(*this, other, args,
                                                      other.args);
}

core::LogicalResult<void> BitType::isEqual(const BitType &) const {
  return success();
}

core::LogicalResult<void> VoidType::isEqual(const VoidType &) const {
  return success();
}

core::LogicalResult<void> TypelessType::isEqual(const TypelessType &) const {
  return success();
}

core::LogicalResult<void> StructType::isEqual(const StructType &other) const {
  if (name != other.name) {
    return comparisonError("Struct types have different names. `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void> UnionType::isEqual(const UnionType &other) const {
  if (name != other.name) {
    return comparisonError("Union types have different names. `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void>
AffecExpression::isEqual(const AffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
AddAffecExpression::isEqual(const AddAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
SubAffecExpression::isEqual(const SubAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
MulAffecExpression::isEqual(const MulAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
ModAffecExpression::isEqual(const ModAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
DivAffecExpression::isEqual(const DivAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LandAffecExpression::isEqual(const LandAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LorAffecExpression::isEqual(const LorAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LxorAffecExpression::isEqual(const LxorAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
BandAffecExpression::isEqual(const BandAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
BorAffecExpression::isEqual(const BorAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
BxorAffecExpression::isEqual(const BxorAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LshiftAffecExpression::isEqual(const LshiftAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
RshiftAffecExpression::isEqual(const RshiftAffecExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LxorExpression::isEqual(const LxorExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LorExpression::isEqual(const LorExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LandExpression::isEqual(const LandExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
BorExpression::isEqual(const BorExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
BxorExpression::isEqual(const BxorExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
BandExpression::isEqual(const BandExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
NeqExpression::isEqual(const NeqExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
EqExpression::isEqual(const EqExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LtExpression::isEqual(const LtExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LeExpression::isEqual(const LeExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
GtExpression::isEqual(const GtExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
GeExpression::isEqual(const GeExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
LshiftExpression::isEqual(const LshiftExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
RshiftExpression::isEqual(const RshiftExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
AddExpression::isEqual(const AddExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
SubExpression::isEqual(const SubExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
DivExpression::isEqual(const DivExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
ModExpression::isEqual(const ModExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
MulExpression::isEqual(const MulExpression &other) const {
  return binexprIsEqual(*this, other);
}

core::LogicalResult<void>
TernaryExpression::isEqual(const TernaryExpression &other) const {
  auto cmp = cond->isEqual(*other.cond);
  if (cmp.failed())
    return cmp;
  cmp = thenPart->isEqual(*other.thenPart);
  if (cmp.failed())
    return cmp;
  return elsePart->isEqual(*other.elsePart);
}

core::LogicalResult<void>
PreIncrExpression::isEqual(const PreIncrExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
PreDecrExpression::isEqual(const PreDecrExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
PostIncrExpression::isEqual(const PostIncrExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
PostDecrExpression::isEqual(const PostDecrExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
DerefExpression::isEqual(const DerefExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
AddrofExpression::isEqual(const AddrofExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
BnotExpression::isEqual(const BnotExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
LnotExpression::isEqual(const LnotExpression &other) const {
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
SyscallExpression::isEqual(const SyscallExpression &other) const {
  auto cmp = code->isEqual(*other.code);
  if (cmp.failed())
    return cmp;
  return compareVector<"Syscall expressions", "arguments">(*this, other, args,
                                                           other.args);
}

core::LogicalResult<void>
SizeofExpression::isEqual(const SizeofExpression &other) const {
  return arg->isEqual(*other.arg);
}

core::LogicalResult<void>
ArrayExpression::isEqual(const ArrayExpression &other) const {
  auto cmp = base->isEqual(*other.base);
  if (cmp.failed())
    return cmp;
  return index->isEqual(*other.index);
}

core::LogicalResult<void>
PtrFieldExpression::isEqual(const PtrFieldExpression &other) const {
  if (name != other.name) {
    return comparisonError("Pointer-fields have different names. `" + name +
                               "` vs `" + other.name + "`.",
                           *this, other);
  }
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
FieldExpression::isEqual(const FieldExpression &other) const {
  if (name != other.name) {
    return comparisonError("Fields have different names. `" + name + "` vs `" +
                               other.name + "`.",
                           *this, other);
  }
  return base->isEqual(*other.base);
}

core::LogicalResult<void>
FunExpression::isEqual(const FunExpression &other) const {
  auto cmp = base->isEqual(*other.base);
  if (cmp.failed())
    return cmp;
  return compareVector<"Function expressions", "arguments">(*this, other, args,
                                                            other.args);
}

core::LogicalResult<void>
CastExpression::isEqual(const CastExpression &other) const {
  auto cmp = base->isEqual(*other.base);
  if (cmp.failed())
    return cmp;
  return newType->isEqual(*other.newType);
}

core::LogicalResult<void>
BitCastExpression::isEqual(const BitCastExpression &other) const {
  auto cmp = base->isEqual(*other.base);
  if (cmp.failed())
    return cmp;
  return newType->isEqual(*other.newType);
}

core::LogicalResult<void>
VarExpression::isEqual(const VarExpression &other) const {
  if (name != other.name) {
    return comparisonError("Variable expressions have different names. `" +
                               name + "` vs `" + other.name + "`.",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void>
IntExpression::isEqual(const IntExpression &other) const {
  if (value != other.value) {
    return comparisonError("Integer expression have different values. " +
                               core::String::of(value) + " vs " +
                               core::String::of(other.value) + ".",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void>
CharExpression::isEqual(const CharExpression &other) const {
  if (value != other.value) {
    return comparisonError("Char expression have different values. '" +
                               core::String(value) + "' vs '" +
                               core::String(other.value) + "'.",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void>
StringExpression::isEqual(const StringExpression &other) const {
  if (value != other.value) {
    return comparisonError("String expression have different values. \"" +
                               core::String(value) + "\" vs \"" +
                               core::String(other.value) + "\".",
                           *this, other);
  }
  return success();
}

core::LogicalResult<void>
TrueExpression::isEqual(const TrueExpression &) const {
  return success();
}

core::LogicalResult<void>
FalseExpression::isEqual(const FalseExpression &) const {
  return success();
}

core::LogicalResult<void>
ThisExpression::isEqual(const ThisExpression &) const {
  return success();
}

core::LogicalResult<void>
OtherExpression::isEqual(const OtherExpression &) const {
  return success();
}

core::LogicalResult<void> ProgramItem::isEqual(const ProgramItem &other) const {
  if (index != other.index) {
    return comparisonError("Program items are different item-types.", *this,
                           other);
  }
  return visit([&]<typename T>(const T &elt) {
    return elt.isEqual(other.get<const T>());
  });
}

core::LogicalResult<void> Type::isEqual(const Type &other) const {
  if (index != other.index) {
    return comparisonError("Types are different type-types.", *this, other);
  }
  return visit([&]<typename T>(const T &elt) {
    return elt.isEqual(other.get<const T>());
  });
}

core::LogicalResult<void> Statement::isEqual(const Statement &other) const {
  if (index != other.index) {
    return comparisonError("Statements are different statement-types.", *this,
                           other);
  }
  return visit([&]<typename T>(const T &elt) {
    return elt.isEqual(other.get<const T>());
  });
}

core::LogicalResult<void> SwitchItem::isEqual(const SwitchItem &other) const {
  if (index != other.index) {
    return comparisonError("Switch items are different item-types.", *this,
                           other);
  }
  return visit([&]<typename T>(const T &elt) {
    return elt.isEqual(other.get<const T>());
  });
}

core::LogicalResult<void> Expression::isEqual(const Expression &other) const {
  if (index != other.index) {
    return comparisonError("Expressions are different expression-types.", *this,
                           other);
  }
  return visit([&]<typename T>(const T &elt) {
    return elt.isEqual(other.get<const T>());
  });
}

} // namespace ast