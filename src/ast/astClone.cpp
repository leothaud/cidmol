module ast;

import :ast;
import core;

namespace ast {

void Node::cloneHeader(Node &other) const {
  other.line_start = line_start;
  other.column_start = column_start;
  other.line_end = line_end;
  other.column_end = column_end;
}

namespace {

template <typename T>
void cloneVector(const core::Vector<core::UniquePtr<T>> &from,
                 core::Vector<core::UniquePtr<T>> &to) {
  for (auto &item : from) {
    to.pushBack(item->clone());
  }
}

template <typename T> core::UniquePtr<T> cloneBinexpr(const T &binexpr) {
  auto result = core::UniquePtr<T>::create();
  binexpr.cloneHeader(*result);
  result->lval = binexpr.lval->clone();
  result->rval = binexpr.rval->clone();
  return result;
}

} // namespace

core::UniquePtr<Program> Program::clone() const {
  auto result = core::UniquePtr<Program>::create();
  cloneHeader(*result);
  cloneVector(items, result->items);
  return result;
}

core::UniquePtr<FunctionDeclaration> FunctionDeclaration::clone() const {
  auto result = core::UniquePtr<FunctionDeclaration>::create();
  cloneHeader(*result);
  result->returnType = returnType->clone();
  result->name = name;
  cloneVector(args, result->args);
  result->body = body->clone();
  return result;
}

core::UniquePtr<Argument> Argument::clone() const {
  auto result = core::UniquePtr<Argument>::create();
  cloneHeader(*result);
  result->type = type->clone();
  result->name = name;
  return result;
}

core::UniquePtr<StructDeclaration> StructDeclaration::clone() const {
  auto result = core::UniquePtr<StructDeclaration>::create();
  cloneHeader(*result);
  result->name = name;
  cloneVector(fields, result->fields);
  cloneVector(ops, result->ops);
  cloneVector(methods, result->methods);
  cloneVector(casts, result->casts);
  return result;
}

core::UniquePtr<UnionDeclaration> UnionDeclaration::clone() const {
  auto result = core::UniquePtr<UnionDeclaration>::create();
  cloneHeader(*result);
  result->name = name;
  cloneVector(fields, result->fields);
  return result;
}

core::UniquePtr<CastDeclaration> CastDeclaration::clone() const {
  auto result = core::UniquePtr<CastDeclaration>::create();
  cloneHeader(*result);
  result->type = type->clone();
  result->body = body->clone();
  return result;
}

core::UniquePtr<OperationDeclaration> OperationDeclaration::clone() const {
  auto result = core::UniquePtr<OperationDeclaration>::create();
  cloneHeader(*result);
  result->opType = opType;
  result->body = body->clone();
  return result;
}

core::UniquePtr<VariableDeclaration> VariableDeclaration::clone() const {
  auto result = core::UniquePtr<VariableDeclaration>::create();
  cloneHeader(*result);
  result->varType = varType->clone();
  cloneVector(decls, result->decls);
  return result;
}

core::UniquePtr<Declarator> Declarator::clone() const {
  auto result = core::UniquePtr<Declarator>::create();
  cloneHeader(*result);
  result->name = name;
  result->value = value ? value->clone() : nullptr;
  return result;
}

core::UniquePtr<CompoundStmt> CompoundStmt::clone() const {
  auto result = core::UniquePtr<CompoundStmt>::create();
  cloneHeader(*result);
  cloneVector(items, result->items);
  return result;
}

core::UniquePtr<WhileStmt> WhileStmt::clone() const {
  auto result = core::UniquePtr<WhileStmt>::create();
  cloneHeader(*result);
  result->cond = cond->clone();
  result->body = body->clone();
  return result;
}

core::UniquePtr<DoWhileStmt> DoWhileStmt::clone() const {
  auto result = core::UniquePtr<DoWhileStmt>::create();
  cloneHeader(*result);
  result->cond = cond->clone();
  result->body = body->clone();
  return result;
}

core::UniquePtr<ForStmt> ForStmt::clone() const {
  auto result = core::UniquePtr<ForStmt>::create();
  cloneHeader(*result);
  result->init = init ? init->clone() : nullptr;
  result->cond = cond ? cond->clone() : nullptr;
  result->step = step ? step->clone() : nullptr;
  result->body = body->clone();
  return result;
}

core::UniquePtr<IfStmt> IfStmt::clone() const {
  auto result = core::UniquePtr<IfStmt>::create();
  cloneHeader(*result);
  result->cond = cond->clone();
  result->thenPart = thenPart->clone();
  result->elsePart = elsePart ? elsePart->clone() : nullptr;
  return result;
}

core::UniquePtr<SwitchStmt> SwitchStmt::clone() const {
  auto result = core::UniquePtr<SwitchStmt>::create();
  cloneHeader(*result);
  result->cond = cond->clone();
  cloneVector(items, result->items);
  return result;
}

core::UniquePtr<SwitchCase> SwitchCase::clone() const {
  auto result = core::UniquePtr<SwitchCase>::create();
  cloneHeader(*result);
  result->cond = cond->clone();
  result->body = body->clone();
  return result;
}

core::UniquePtr<SwitchDefault> SwitchDefault::clone() const {
  auto result = core::UniquePtr<SwitchDefault>::create();
  cloneHeader(*result);
  result->body = body->clone();
  return result;
}

core::UniquePtr<ExpressionStmt> ExpressionStmt::clone() const {
  auto result = core::UniquePtr<ExpressionStmt>::create();
  cloneHeader(*result);
  result->value = value->clone();
  return result;
}

core::UniquePtr<NopStmt> NopStmt::clone() const {
  auto result = core::UniquePtr<NopStmt>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<ContinueStmt> ContinueStmt::clone() const {
  auto result = core::UniquePtr<ContinueStmt>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<BreakStmt> BreakStmt::clone() const {
  auto result = core::UniquePtr<BreakStmt>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<ReturnStmt> ReturnStmt::clone() const {
  auto result = core::UniquePtr<ReturnStmt>::create();
  cloneHeader(*result);
  result->value = value ? value->clone() : nullptr;
  return result;
}

core::UniquePtr<PtrType> PtrType::clone() const {
  auto result = core::UniquePtr<PtrType>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<ArrayType> ArrayType::clone() const {
  auto result = core::UniquePtr<ArrayType>::create();
  cloneHeader(*result);
  result->base = base->clone();
  result->size = size->clone();
  return result;
}

core::UniquePtr<FunType> FunType::clone() const {
  auto result = core::UniquePtr<FunType>::create();
  cloneHeader(*result);
  result->base = base->clone();
  cloneVector(args, result->args);
  return result;
}

core::UniquePtr<BitType> BitType::clone() const {
  auto result = core::UniquePtr<BitType>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<VoidType> VoidType::clone() const {
  auto result = core::UniquePtr<VoidType>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<TypelessType> TypelessType::clone() const {
  auto result = core::UniquePtr<TypelessType>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<StructType> StructType::clone() const {
  auto result = core::UniquePtr<StructType>::create();
  cloneHeader(*result);
  result->name = name;
  return result;
}

core::UniquePtr<UnionType> UnionType::clone() const {
  auto result = core::UniquePtr<UnionType>::create();
  cloneHeader(*result);
  result->name = name;
  return result;
}

core::UniquePtr<AffecExpression> AffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<AddAffecExpression> AddAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<SubAffecExpression> SubAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<MulAffecExpression> MulAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<ModAffecExpression> ModAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<DivAffecExpression> DivAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LandAffecExpression> LandAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LorAffecExpression> LorAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LxorAffecExpression> LxorAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<BandAffecExpression> BandAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<BorAffecExpression> BorAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<BxorAffecExpression> BxorAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LshiftAffecExpression> LshiftAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<RshiftAffecExpression> RshiftAffecExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LxorExpression> LxorExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LorExpression> LorExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LandExpression> LandExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<BorExpression> BorExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<BxorExpression> BxorExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<BandExpression> BandExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<NeqExpression> NeqExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<EqExpression> EqExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LtExpression> LtExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LeExpression> LeExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<GtExpression> GtExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<GeExpression> GeExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<LshiftExpression> LshiftExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<RshiftExpression> RshiftExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<AddExpression> AddExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<SubExpression> SubExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<DivExpression> DivExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<ModExpression> ModExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<MulExpression> MulExpression::clone() const {
  return cloneBinexpr(*this);
}

core::UniquePtr<TernaryExpression> TernaryExpression::clone() const {
  auto result = core::UniquePtr<TernaryExpression>::create();
  cloneHeader(*result);
  result->cond = cond->clone();
  result->thenPart = thenPart->clone();
  result->elsePart = elsePart->clone();
  return result;
}

core::UniquePtr<PreIncrExpression> PreIncrExpression::clone() const {
  auto result = core::UniquePtr<PreIncrExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<PreDecrExpression> PreDecrExpression::clone() const {
  auto result = core::UniquePtr<PreDecrExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<PostIncrExpression> PostIncrExpression::clone() const {
  auto result = core::UniquePtr<PostIncrExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<PostDecrExpression> PostDecrExpression::clone() const {
  auto result = core::UniquePtr<PostDecrExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<DerefExpression> DerefExpression::clone() const {
  auto result = core::UniquePtr<DerefExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<AddrofExpression> AddrofExpression::clone() const {
  auto result = core::UniquePtr<AddrofExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<LnotExpression> LnotExpression::clone() const {
  auto result = core::UniquePtr<LnotExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<BnotExpression> BnotExpression::clone() const {
  auto result = core::UniquePtr<BnotExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  return result;
}

core::UniquePtr<SyscallExpression> SyscallExpression::clone() const {
  auto result = core::UniquePtr<SyscallExpression>::create();
  cloneHeader(*result);
  result->code = code->clone();
  cloneVector(args, result->args);
  return result;
}

core::UniquePtr<SizeofExpression> SizeofExpression::clone() const {
  auto result = core::UniquePtr<SizeofExpression>::create();
  cloneHeader(*result);
  result->arg = arg->clone();
  return result;
}

core::UniquePtr<ArrayExpression> ArrayExpression::clone() const {
  auto result = core::UniquePtr<ArrayExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  result->index = index->clone();
  return result;
}

core::UniquePtr<PtrFieldExpression> PtrFieldExpression::clone() const {
  auto result = core::UniquePtr<PtrFieldExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  result->name = name;
  return result;
}

core::UniquePtr<FieldExpression> FieldExpression::clone() const {
  auto result = core::UniquePtr<FieldExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  result->name = name;
  return result;
}

core::UniquePtr<FunExpression> FunExpression::clone() const {
  auto result = core::UniquePtr<FunExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  cloneVector(args, result->args);
  return result;
}

core::UniquePtr<CastExpression> CastExpression::clone() const {
  auto result = core::UniquePtr<CastExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  result->newType = newType->clone();
  return result;
}

core::UniquePtr<BitCastExpression> BitCastExpression::clone() const {
  auto result = core::UniquePtr<BitCastExpression>::create();
  cloneHeader(*result);
  result->base = base->clone();
  result->newType = newType->clone();
  return result;
}

core::UniquePtr<VarExpression> VarExpression::clone() const {
  auto result = core::UniquePtr<VarExpression>::create();
  cloneHeader(*result);
  result->name = name;
  return result;
}

core::UniquePtr<IntExpression> IntExpression::clone() const {
  auto result = core::UniquePtr<IntExpression>::create();
  cloneHeader(*result);
  result->value = value;
  return result;
}

core::UniquePtr<CharExpression> CharExpression::clone() const {
  auto result = core::UniquePtr<CharExpression>::create();
  cloneHeader(*result);
  result->value = value;
  return result;
}

core::UniquePtr<StringExpression> StringExpression::clone() const {
  auto result = core::UniquePtr<StringExpression>::create();
  cloneHeader(*result);
  result->value = value;
  return result;
}

core::UniquePtr<TrueExpression> TrueExpression::clone() const {
  auto result = core::UniquePtr<TrueExpression>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<FalseExpression> FalseExpression::clone() const {
  auto result = core::UniquePtr<FalseExpression>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<ThisExpression> ThisExpression::clone() const {
  auto result = core::UniquePtr<ThisExpression>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<OtherExpression> OtherExpression::clone() const {
  auto result = core::UniquePtr<OtherExpression>::create();
  cloneHeader(*result);

  return result;
}

core::UniquePtr<ProgramItem> ProgramItem::clone() const {
  return this->visit([&](auto &&elt) {
    auto cloned = elt.clone();
    auto result = core::UniquePtr<ProgramItem>::create();
    *result = core::move(*cloned);
    return result;
  });
}

core::UniquePtr<Type> Type::clone() const {
  return this->visit([&](auto &&elt) {
    auto cloned = elt.clone();
    auto result = core::UniquePtr<Type>::create();
    *result = core::move(*cloned);
    return result;
  });
}

core::UniquePtr<Statement> Statement::clone() const {
  return this->visit([&](auto &&elt) {
    auto cloned = elt.clone();
    auto result = core::UniquePtr<Statement>::create();
    *result = core::move(*cloned);
    return result;
  });
}

core::UniquePtr<SwitchItem> SwitchItem::clone() const {
  return this->visit([&](auto &&elt) {
    auto cloned = elt.clone();
    auto result = core::UniquePtr<SwitchItem>::create();
    *result = core::move(*cloned);
    return result;
  });
}

core::UniquePtr<Expression> Expression::clone() const {
  return this->visit([&](auto &&elt) {
    auto cloned = elt.clone();
    auto result = core::UniquePtr<Expression>::create();
    *result = core::move(*cloned);
    return result;
  });
}

} // namespace ast