module ast;

import :ast;
import core;

namespace ast {

core::LogicalResult<void> Node::error(core::String message) const {
  return core::LogicalResult<void>::failure(
      "Error at: " + core::String::of(line_start) + ":" +
      core::String::of(column_start) + "-" + core::String::of(line_end) + ":" +
      core::String::of(column_end) + ":\n" + message);
}

core::LogicalResult<void> Program::verify() const {
  for (auto &item : items) {
    if (item == nullptr) {
      return error("Program item is null");
    }
    auto itemverify = item->verify();
    if (itemverify.failed())
      return itemverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> FunctionDeclaration::verify() const {
  if (returnType == nullptr) {
    return error("Function declaration return type is null.");
  }
  if (name == "") {
    return error("Function declaration name is empty.");
  }
  for (auto &arg : args) {
    if (arg == nullptr) {
      return error("Function declaration argument is null.");
    }
    auto argverify = arg->verify();
    if (argverify.failed())
      return argverify;
  }
  if (body == nullptr) {
    return error("Function declaration body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> Argument::verify() const {
  if (name == "") {
    return error("Argument name is empty.");
  }
  if (type == nullptr) {
    return error("Argument type in null.");
  }
  return type->verify();
}

core::LogicalResult<void> StructDeclaration::verify() const {
  if (name == "") {
    return error("Struct declaration name is empty.");
  }
  for (auto &f : fields) {
    if (f == nullptr) {
      return error("Struct declaration field is null.");
    }
    auto fieldverify = f->verify();
    if (fieldverify.failed())
      return fieldverify;
  }
  for (auto &o : ops) {
    if (o == nullptr) {
      return error("Struct declaration operator is null.");
    }
    auto opverify = o->verify();
    if (opverify.failed())
      return opverify;
  }
  for (auto &m : methods) {
    if (m == nullptr) {
      return error("Struct declaration method is null.");
    }
    auto methodverify = m->verify();
    if (methodverify.failed())
      return methodverify;
  }
  for (auto &c : casts) {
    if (c == nullptr) {
      return error("Struct declaration cast is null.");
    }
    auto castverify = c->verify();
    if (castverify.failed())
      return castverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> UnionDeclaration::verify() const {
  if (name == "")
    return error("Union declaration name is empty.");
  for (auto &f : fields) {
    if (f == nullptr) {
      return error("Union declaration field is null");
      auto fieldverify = f->verify();
      if (fieldverify.failed())
        return fieldverify;
    }
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> CastDeclaration::verify() const {
  if (type == nullptr)
    return error("Cast declaration type is null.");
  auto typeverify = type->verify();
  if (typeverify.failed())
    return typeverify;
  if (body == nullptr) {
    return error("Cast declaration body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> OperationDeclaration::verify() const {
  if (body == nullptr) {
    return error("Operation declaration body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> VariableDeclaration::verify() const {
  if (varType == nullptr) {
    return error("Variable declaration type is null.");
  }
  auto typeverify = varType->verify();
  if (typeverify.failed())
    return typeverify;
  for (auto &d : decls) {
    if (d == nullptr) {
      return error("Declarator is null.");
    }
    auto declverify = d->verify();
    if (declverify.failed())
      return declverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> Declarator::verify() const {
  if (name == "") {
    return error("Declarator name is empty.");
  }
  if (value != nullptr) {
    return value->verify();
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> CompoundStmt::verify() const {
  for (auto &i : items) {
    if (i == nullptr) {
      return error("Compound statement item is empty.");
    }
    auto itemverify = i->verify();
    if (itemverify.failed())
      return itemverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> WhileStmt::verify() const {
  if (cond == nullptr) {
    return error("While loop condition is null.");
  }
  auto condverify = cond->verify();
  if (condverify.failed())
    return condverify;
  if (body == nullptr) {
    return error("While loop body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> DoWhileStmt::verify() const {
  if (cond == nullptr) {
    return error("Do-while loop condition is null.");
  }
  auto condverify = cond->verify();
  if (condverify.failed())
    return condverify;
  if (body == nullptr) {
    return error("Do-while loop body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> ForStmt::verify() const {
  if (init != nullptr) {
    auto initverify = init->verify();
    if (initverify.failed())
      return initverify;
  }
  if (cond != nullptr) {
    auto condverify = cond->verify();
    if (condverify.failed())
      return condverify;
  }
  if (step != nullptr) {
    auto stepverify = step->verify();
    if (stepverify.failed())
      return stepverify;
  }
  if (body == nullptr) {
    return error("For loop body is null.");
    return body->verify();
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> IfStmt::verify() const {
  if (cond == nullptr) {
    return error("If statement condition is null.");
  }
  auto condverify = cond->verify();
  if (condverify.failed())
    return condverify;
  if (thenPart == nullptr) {
    return error("If statement then-part is null.");
  }
  auto thenverify = thenPart->verify();
  if (thenverify.failed())
    return thenverify;
  if (elsePart != nullptr) {
    return elsePart->verify();
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> SwitchStmt::verify() const {
  if (cond == nullptr) {
    return error("Switch statement condition is null.");
  }
  auto condverify = cond->verify();
  if (condverify.failed())
    return condverify;
  for (auto &i : items) {
    if (i == nullptr) {
      return error("Switch statement item is null.");
    }
    auto itemverify = i->verify();
    if (itemverify.failed())
      return itemverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> SwitchCase::verify() const {
  if (cond == nullptr) {
    return error("Switch case condition is null.");
  }
  auto condverify = cond->verify();
  if (condverify.failed())
    return condverify;
  if (body == nullptr) {
    return error("Switch case body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> SwitchDefault::verify() const {
  if (body == nullptr) {
    return error("Switch default body is null.");
  }
  return body->verify();
}

core::LogicalResult<void> ExpressionStmt::verify() const {
  if (value == nullptr) {
    return error("Expression statement value is null.");
  }
  return value->verify();
}

core::LogicalResult<void> NopStmt::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> ContinueStmt::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> BreakStmt::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> ReturnStmt::verify() const {
  if (value != nullptr) {
    return value->verify();
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> PtrType::verify() const {
  if (base == nullptr) {
    return error("Ptr type base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> ArrayType::verify() const {
  if (base == nullptr) {
    return error("Array type base is null.");
  }
  auto baseverify = base->verify();
  if (baseverify.failed())
    return baseverify;
  if (size == nullptr) {
    return error("Array type size is null.");
  }
  return size->verify();
}

core::LogicalResult<void> FunType::verify() const {
  if (base == nullptr) {
    return error("Fun type base is null");
  }
  auto baseverify = base->verify();
  if (baseverify.failed())
    return baseverify;
  for (auto &a : args) {
    if (a == nullptr) {
      return error("Fun type argument is null.");
    }
    auto argverify = a->verify();
    if (argverify.failed())
      return argverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> BitType::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> VoidType::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> TypelessType::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> StructType::verify() const {
  if (name == "") {
    return error("Struct type name is empty.");
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> UnionType::verify() const {
  if (name == "") {
    return error("Union type name is empty.");
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> AffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> AddAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> SubAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> MulAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> ModAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> DivAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LandAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LorAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LxorAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> BandAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> BorAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> BxorAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LshiftAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> RshiftAffecExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LxorExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LorExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LandExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> BorExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> BxorExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> BandExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> NeqExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> EqExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LtExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LeExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> GtExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> GeExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> LshiftExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> RshiftExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> AddExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> SubExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> DivExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> ModExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> MulExpression::verify() const {
  if (lval == nullptr) {
    return error("Binary expression lval is null.");
  }
  auto lvalverify = lval->verify();
  if (lvalverify.failed())
    return lvalverify;
  if (rval == nullptr) {
    return error("Binary expression rval is null.");
  }
  return rval->verify();
}

core::LogicalResult<void> TernaryExpression::verify() const {
  if (cond == nullptr) {
    return error("Ternary expression condition is null.");
  }
  auto condverify = cond->verify();
  if (condverify.failed())
    return condverify;
  if (thenPart == nullptr) {
    return error("Ternary expression then-part is null.");
  }
  auto thenverify = thenPart->verify();
  if (thenverify.failed())
    return thenverify;
  if (elsePart == nullptr) {
    return error("Ternary expression else-part is null.");
  }
  return elsePart->verify();
}

core::LogicalResult<void> PreIncrExpression::verify() const {
  if (base == nullptr) {
    return error("Pre-incr expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> PreDecrExpression::verify() const {
  if (base == nullptr) {
    return error("Pre-decr expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> PostIncrExpression::verify() const {
  if (base == nullptr) {
    return error("Post-incr expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> PostDecrExpression::verify() const {
  if (base == nullptr) {
    return error("Post-decr expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> DerefExpression::verify() const {
  if (base == nullptr) {
    return error("Deref expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> AddrofExpression::verify() const {
  if (base == nullptr) {
    return error("Addrof expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> LnotExpression::verify() const {
  if (base == nullptr) {
    return error("Lnot expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> BnotExpression::verify() const {
  if (base == nullptr) {
    return error("Bnot expression base is null.");
  }
  return base->verify();
}

core::LogicalResult<void> SyscallExpression::verify() const {
  if (code == nullptr) {
    return error("Syscall expression code is null.");
  }
  auto codeverify = code->verify();
  if (codeverify.failed())
    return codeverify;
  for (auto &a : args) {
    if (a == nullptr) {
      return error("Syscall argument is null.");
    }
    auto argverify = a->verify();
    if (argverify.failed())
      return argverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> SizeofExpression::verify() const {
  if (arg == nullptr) {
    return error("Sizeof expression type argument is null.");
  }
  return arg->verify();
}

core::LogicalResult<void> ArrayExpression::verify() const {
  if (base == nullptr) {
    return error("Array expression base is null.");
  }
  auto baseVerif = base->verify();
  if (baseVerif.failed())
    return baseVerif;
  if (index == nullptr) {
    return error("Array expression index is null.");
  }
  return index->verify();
}

core::LogicalResult<void> PtrFieldExpression::verify() const {
  if (base == nullptr) {
    return error("Ptr field expression base is null.");
  }
  if (name == "") {
    return error("Ptr field expression name is null.");
  }
  return base->verify();
}

core::LogicalResult<void> FieldExpression::verify() const {
  if (base == nullptr) {
    return error("Field expression base is null.");
  }
  if (name == "") {
    return error("Field expression name is null.");
  }
  return base->verify();
}

core::LogicalResult<void> FunExpression::verify() const {
  if (base == nullptr) {
    return error("Fun expression base is null.");
  }
  auto baseverify = base->verify();
  if (baseverify.failed())
    return baseverify;
  for (auto &a : args) {
    if (a == nullptr) {
      return error("Fun expression argument is null.");
    }
    auto argverify = a->verify();
    if (argverify.failed())
      return argverify;
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> CastExpression::verify() const {
  if (base == nullptr) {
    return error("Cast expression base is null.");
  }
  auto baseverify = base->verify();
  if (baseverify.failed())
    return baseverify;
  if (newType == nullptr) {
    return error("Cast expression new type is null.");
  }
  return newType->verify();
}

core::LogicalResult<void> BitCastExpression::verify() const {
  if (base == nullptr) {
    return error("Bit-cast expression base is null.");
  }
  auto baseverify = base->verify();
  if (baseverify.failed())
    return baseverify;
  if (newType == nullptr) {
    return error("Bit-cast expression new type is null.");
  }
  return newType->verify();
}

core::LogicalResult<void> VarExpression::verify() const {
  if (name == "") {
    return error("Variable expression name is empty.");
  }
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> IntExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> CharExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> StringExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> TrueExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> FalseExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> ThisExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> OtherExpression::verify() const {
  return core::LogicalResult<void>::success();
}

core::LogicalResult<void> ProgramItem::verify() const {
  return this->visit([](auto &&elt) { return elt.verify(); });
}
core::LogicalResult<void> Type::verify() const {
  return this->visit([](auto &&elt) { return elt.verify(); });
}
core::LogicalResult<void> Statement::verify() const {
  return this->visit([](auto &&elt) { return elt.verify(); });
}
core::LogicalResult<void> SwitchItem::verify() const {
  return this->visit([](auto &&elt) { return elt.verify(); });
}
core::LogicalResult<void> Expression::verify() const {
  return this->visit([](auto &&elt) { return elt.verify(); });
}

} // namespace ast