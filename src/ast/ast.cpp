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
export module ast:ast;

import core;
import :symbolTable;
import :fwd;

namespace ast {

export struct Node {
  u64 line_start, column_start, line_end, column_end;
  core::LogicalResult<void> error(core::String message) const;
  core::SharedPtr<symbolTable::Level> associatedLevel;

  Node(u64 line_start, u64 column_start, u64 line_end, u64 column_end);
  Node();

  void cloneHeader(Node &other) const;

  u64 getLineStart() const;
  u64 getColumnStart() const;
  u64 getLineEnd() const;
  u64 getColumnEnd() const;
};

export struct ProgramItem;
export struct Type;
export struct Statement;
export struct SwitchItem;
export struct Expression;
export struct Program;
export struct FunctionDeclaration;
export struct Argument;
export struct StructDeclaration;
export struct UnionDeclaration;
export struct CastDeclaration;
export struct OperationDeclaration;
export struct VariableDeclaration;
export struct Declarator;
export struct CompoundStmt;
export struct WhileStmt;
export struct DoWhileStmt;
export struct ForStmt;
export struct IfStmt;
export struct SwitchStmt;
export struct SwitchCase;
export struct SwitchDefault;
export struct ExpressionStmt;
export struct NopStmt;
export struct ContinueStmt;
export struct BreakStmt;
export struct ReturnStmt;
export struct PtrType;
export struct ArrayType;
export struct FunType;
export struct BitType;
export struct VoidType;
export struct TypelessType;
export struct StructType;
export struct UnionType;
export struct AffecExpression;
export struct AddAffecExpression;
export struct SubAffecExpression;
export struct MulAffecExpression;
export struct ModAffecExpression;
export struct DivAffecExpression;
export struct LandAffecExpression;
export struct LorAffecExpression;
export struct LxorAffecExpression;
export struct BandAffecExpression;
export struct BorAffecExpression;
export struct BxorAffecExpression;
export struct LshiftAffecExpression;
export struct RshiftAffecExpression;
export struct LxorExpression;
export struct LorExpression;
export struct LandExpression;
export struct BorExpression;
export struct BxorExpression;
export struct BandExpression;
export struct NeqExpression;
export struct EqExpression;
export struct LtExpression;
export struct LeExpression;
export struct GtExpression;
export struct GeExpression;
export struct LshiftExpression;
export struct RshiftExpression;
export struct AddExpression;
export struct SubExpression;
export struct DivExpression;
export struct ModExpression;
export struct MulExpression;
export struct TernaryExpression;
export struct PreIncrExpression;
export struct PreDecrExpression;
export struct PostIncrExpression;
export struct PostDecrExpression;
export struct DerefExpression;
export struct AddrofExpression;
export struct SyscallExpression;
export struct SizeofExpression;
export struct PtrFieldExpression;
export struct FieldExpression;
export struct FunExpression;
export struct CastExpression;
export struct BitCastExpression;
export struct VarExpression;
export struct IntExpression;
export struct CharExpression;
export struct StringExpression;
export struct TrueExpression;
export struct FalseExpression;
export struct ThisExpression;
export struct OtherExpression;

export struct Program : public Node {
  core::Vector<core::UniquePtr<ProgramItem>> items;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<Program> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const Program &other) const;
};

export struct FunctionDeclaration : public Node {
  core::UniquePtr<Type> returnType;
  core::String name;
  core::Vector<core::UniquePtr<Argument>> args;
  core::UniquePtr<CompoundStmt> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<FunctionDeclaration> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const FunctionDeclaration &other) const;
};

export struct Argument : public Node {
  core::UniquePtr<Type> type;
  core::String name;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<Argument> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const Argument &other) const;
};

export struct StructDeclaration : public Node {
  core::String name;
  core::Vector<core::UniquePtr<VariableDeclaration>> fields;
  core::Vector<core::UniquePtr<OperationDeclaration>> ops;
  core::Vector<core::UniquePtr<FunctionDeclaration>> methods;
  core::Vector<core::UniquePtr<CastDeclaration>> casts;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<StructDeclaration> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const StructDeclaration &other) const;
};

export struct UnionDeclaration : public Node {
  core::String name;
  core::Vector<core::UniquePtr<Argument>> fields;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<UnionDeclaration> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const UnionDeclaration &other) const;
};

export struct CastDeclaration : public Node {
  core::UniquePtr<Type> type;
  core::UniquePtr<CompoundStmt> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<CastDeclaration> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const CastDeclaration &other) const;
};

export struct OperationDeclaration : public Node {
  enum OperatorType {
    ADD_OP,
    SUB_OP,
    MUL_OP,
    MOD_OP,
    DIV_OP,
    LAND_OP,
    LOR_OP,
    LXOR_OP,
    LNOT_OP,
    EQ_OP,
    LT_OP
  };
  static core::String to_string(OperatorType opType) {
    switch (opType) {
    case ADD_OP:
      return "+";
    case SUB_OP:
      return "-";
    case MUL_OP:
      return "*";
    case MOD_OP:
      return "%";
    case DIV_OP:
      return "/";
    case LAND_OP:
      return "&&";
    case LOR_OP:
      return "||";
    case LXOR_OP:
      return "^^";
    case LNOT_OP:
      return "!";
    case EQ_OP:
      return "==";
    case LT_OP:
      return "<";
    }
  }

  OperatorType opType;
  core::UniquePtr<CompoundStmt> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<OperationDeclaration> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const OperationDeclaration &other) const;
};

export struct VariableDeclaration : public Node {
  core::UniquePtr<Type> varType;
  core::Vector<core::UniquePtr<Declarator>> decls;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<VariableDeclaration> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const VariableDeclaration &other) const;
};

export struct Declarator : public Node {
  core::String name;
  core::UniquePtr<Expression> value;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<Declarator> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const Declarator &other) const;
};

export struct CompoundStmt : public Node {
  core::Vector<core::UniquePtr<Statement>> items;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<CompoundStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const CompoundStmt &other) const;
};

export struct WhileStmt : public Node {
  core::UniquePtr<Expression> cond;
  core::UniquePtr<Statement> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<WhileStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const WhileStmt &other) const;
};

export struct DoWhileStmt : public Node {
  core::UniquePtr<Expression> cond;
  core::UniquePtr<Statement> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<DoWhileStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const DoWhileStmt &other) const;
};

export struct ForStmt : public Node {
  core::UniquePtr<VariableDeclaration> init;
  core::UniquePtr<Expression> cond, step;
  core::UniquePtr<Statement> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ForStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ForStmt &other) const;
};

export struct IfStmt : public Node {
  core::UniquePtr<Expression> cond;
  core::UniquePtr<Statement> thenPart, elsePart;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<IfStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const IfStmt &other) const;
};

export struct SwitchStmt : public Node {
  core::UniquePtr<Expression> cond;
  core::Vector<core::UniquePtr<SwitchItem>> items;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SwitchStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SwitchStmt &other) const;
};

export struct SwitchCase : public Node {
  core::UniquePtr<Expression> cond;
  core::UniquePtr<Statement> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SwitchCase> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SwitchCase &other) const;
};

export struct SwitchDefault : public Node {
  core::UniquePtr<Statement> body;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SwitchDefault> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SwitchDefault &other) const;
};

export struct ExpressionStmt : public Node {
  core::UniquePtr<Expression> value;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ExpressionStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ExpressionStmt &other) const;
};

export struct NopStmt : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<NopStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const NopStmt &other) const;
};

export struct ContinueStmt : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ContinueStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ContinueStmt &other) const;
};

export struct BreakStmt : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BreakStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BreakStmt &other) const;
};

export struct ReturnStmt : public Node {
  core::UniquePtr<Expression> value;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ReturnStmt> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ReturnStmt &other) const;
};

export struct PtrType : public Node {
  core::UniquePtr<Type> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<PtrType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const PtrType &other) const;
};

export struct ArrayType : public Node {
  core::UniquePtr<Type> base;
  core::UniquePtr<Expression> size;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ArrayType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ArrayType &other) const;
};

export struct FunType : public Node {
  core::UniquePtr<Type> base;
  core::Vector<core::UniquePtr<Type>> args;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<FunType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const FunType &other) const;
};

export struct BitType : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BitType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BitType &other) const;
};

export struct VoidType : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<VoidType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const VoidType &other) const;
};

export struct TypelessType : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<TypelessType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const TypelessType &other) const;
};

export struct StructType : public Node {
  core::String name;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<StructType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const StructType &other) const;
};

export struct UnionType : public Node {
  core::String name;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<UnionType> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const UnionType &other) const;
};

export struct AffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<AffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const AffecExpression &other) const;
};

export struct AddAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<AddAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const AddAffecExpression &other) const;
};

export struct SubAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SubAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SubAffecExpression &other) const;
};

export struct MulAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<MulAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const MulAffecExpression &other) const;
};

export struct ModAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ModAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ModAffecExpression &other) const;
};

export struct DivAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<DivAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const DivAffecExpression &other) const;
};

export struct LandAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LandAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LandAffecExpression &other) const;
};

export struct LorAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LorAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LorAffecExpression &other) const;
};

export struct LxorAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LxorAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LxorAffecExpression &other) const;
};

export struct BandAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BandAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BandAffecExpression &other) const;
};

export struct BorAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BorAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BorAffecExpression &other) const;
};

export struct BxorAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BxorAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BxorAffecExpression &other) const;
};

export struct LshiftAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LshiftAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LshiftAffecExpression &other) const;
};

export struct RshiftAffecExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<RshiftAffecExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const RshiftAffecExpression &other) const;
};

export struct LxorExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LxorExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LxorExpression &other) const;
};

export struct LorExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LorExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LorExpression &other) const;
};

export struct LandExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LandExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LandExpression &other) const;
};

export struct BorExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BorExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BorExpression &other) const;
};

export struct BxorExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BxorExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BxorExpression &other) const;
};

export struct BandExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BandExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BandExpression &other) const;
};

export struct NeqExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<NeqExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const NeqExpression &other) const;
};

export struct EqExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<EqExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const EqExpression &other) const;
};

export struct LtExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LtExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LtExpression &other) const;
};

export struct LeExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LeExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LeExpression &other) const;
};

export struct GtExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<GtExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const GtExpression &other) const;
};

export struct GeExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<GeExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const GeExpression &other) const;
};

export struct LshiftExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LshiftExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LshiftExpression &other) const;
};

export struct RshiftExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<RshiftExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const RshiftExpression &other) const;
};

export struct AddExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<AddExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const AddExpression &other) const;
};

export struct SubExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SubExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SubExpression &other) const;
};

export struct DivExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<DivExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const DivExpression &other) const;
};

export struct ModExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ModExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ModExpression &other) const;
};

export struct MulExpression : public Node {
  core::UniquePtr<Expression> lval, rval;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<MulExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const MulExpression &other) const;
};

export struct TernaryExpression : public Node {
  core::UniquePtr<Expression> cond, thenPart, elsePart;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<TernaryExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const TernaryExpression &other) const;
};

export struct PreIncrExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<PreIncrExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const PreIncrExpression &other) const;
};

export struct PreDecrExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<PreDecrExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const PreDecrExpression &other) const;
};

export struct PostIncrExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<PostIncrExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const PostIncrExpression &other) const;
};

export struct PostDecrExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<PostDecrExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const PostDecrExpression &other) const;
};

export struct DerefExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<DerefExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const DerefExpression &other) const;
};

export struct AddrofExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<AddrofExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const AddrofExpression &other) const;
};

export struct BnotExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BnotExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BnotExpression &other) const;
};

export struct LnotExpression : public Node {
  core::UniquePtr<Expression> base;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<LnotExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const LnotExpression &other) const;
};

export struct SyscallExpression : public Node {
  core::UniquePtr<Expression> code;
  core::Vector<core::UniquePtr<Expression>> args;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SyscallExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SyscallExpression &other) const;
};

export struct SizeofExpression : public Node {
  core::UniquePtr<Type> arg;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<SizeofExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const SizeofExpression &other) const;
};

export struct ArrayExpression : public Node {
  core::UniquePtr<Expression> base, index;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ArrayExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ArrayExpression &other) const;
};

export struct PtrFieldExpression : public Node {
  core::UniquePtr<Expression> base;
  core::String name;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<PtrFieldExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const PtrFieldExpression &other) const;
};

export struct FieldExpression : public Node {
  core::UniquePtr<Expression> base;
  core::String name;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<FieldExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const FieldExpression &other) const;
};

export struct FunExpression : public Node {
  core::UniquePtr<Expression> base;
  core::Vector<core::UniquePtr<Expression>> args;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<FunExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const FunExpression &other) const;
};

export struct CastExpression : public Node {
  core::UniquePtr<Expression> base;
  core::UniquePtr<Type> newType;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<CastExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const CastExpression &other) const;
};

export struct BitCastExpression : public Node {
  core::UniquePtr<Expression> base;
  core::UniquePtr<Type> newType;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<BitCastExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const BitCastExpression &other) const;
};

export struct VarExpression : public Node {
  core::String name;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<VarExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const VarExpression &other) const;
};

export struct IntExpression : public Node {
  u64 value;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<IntExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const IntExpression &other) const;
};

export struct CharExpression : public Node {
  char value;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<CharExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const CharExpression &other) const;
};

export struct StringExpression : public Node {
  core::String value;

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<StringExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const StringExpression &other) const;
};

export struct TrueExpression : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<TrueExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const TrueExpression &other) const;
};

export struct FalseExpression : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<FalseExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const FalseExpression &other) const;
};

export struct ThisExpression : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<ThisExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const ThisExpression &other) const;
};

export struct OtherExpression : public Node {

public:
  core::LogicalResult<void> verify() const;
  core::UniquePtr<OtherExpression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  core::LogicalResult<void> isEqual(const OtherExpression &other) const;
};

export struct ProgramItem
    : public core::Variant<FunctionDeclaration, StructDeclaration,
                           UnionDeclaration, VariableDeclaration> {

  using Variant::Variant;

  core::LogicalResult<void> verify() const;
  core::UniquePtr<ProgramItem> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  u64 getLineStart() const;
  u64 getColumnStart() const;
  u64 getLineEnd() const;
  u64 getColumnEnd() const;

  core::LogicalResult<void> isEqual(const ProgramItem &other) const;
};

export struct Type
    : public core::Variant<PtrType, ArrayType, FunType, BitType, VoidType,
                           TypelessType, StructType, UnionType> {

  using Variant::Variant;

  core::LogicalResult<void> verify() const;
  core::UniquePtr<Type> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  u64 getLineStart() const;
  u64 getColumnStart() const;
  u64 getLineEnd() const;
  u64 getColumnEnd() const;

  core::LogicalResult<void> isEqual(const Type &other) const;
};

export struct Statement
    : core::Variant<VariableDeclaration, CompoundStmt, WhileStmt, DoWhileStmt,
                    ForStmt, IfStmt, SwitchStmt, ExpressionStmt, NopStmt,
                    ContinueStmt, BreakStmt, ReturnStmt> {

  using Variant::Variant;

  core::LogicalResult<void> verify() const;
  core::UniquePtr<Statement> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  u64 getLineStart() const;
  u64 getColumnStart() const;
  u64 getLineEnd() const;
  u64 getColumnEnd() const;

  core::LogicalResult<void> isEqual(const Statement &other) const;
};

export struct SwitchItem : core::Variant<SwitchCase, SwitchDefault> {

  using Variant::Variant;

  core::LogicalResult<void> verify() const;
  core::UniquePtr<SwitchItem> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  u64 getLineStart() const;
  u64 getColumnStart() const;
  u64 getLineEnd() const;
  u64 getColumnEnd() const;

  core::LogicalResult<void> isEqual(const SwitchItem &other) const;
};

export struct Expression
    : core::Variant<
          AffecExpression, AddAffecExpression, SubAffecExpression,
          MulAffecExpression, ModAffecExpression, DivAffecExpression,
          LandAffecExpression, LorAffecExpression, LxorAffecExpression,
          BandAffecExpression, BorAffecExpression, BxorAffecExpression,
          LshiftAffecExpression, RshiftAffecExpression, LxorExpression,
          LorExpression, LandExpression, BorExpression, BxorExpression,
          BandExpression, NeqExpression, EqExpression, LtExpression,
          LeExpression, GtExpression, GeExpression, LshiftExpression,
          RshiftExpression, AddExpression, SubExpression, DivExpression,
          ModExpression, MulExpression, TernaryExpression, PreIncrExpression,
          PreDecrExpression, PostIncrExpression, PostDecrExpression,
          DerefExpression, AddrofExpression, BnotExpression, LnotExpression,
          SyscallExpression, SizeofExpression, ArrayExpression,
          PtrFieldExpression, FieldExpression, FunExpression, CastExpression,
          BitCastExpression, VarExpression, IntExpression, CharExpression,
          StringExpression, TrueExpression, FalseExpression, ThisExpression,
          OtherExpression> {

  using Variant::Variant;

  core::LogicalResult<void> verify() const;
  core::UniquePtr<Expression> clone() const;
  template <typename STREAM>
  STREAM &prettyPrint(STREAM &stream, int tab = 0) const;

  u64 getLineStart() const;
  u64 getColumnStart() const;
  u64 getLineEnd() const;
  u64 getColumnEnd() const;

  core::LogicalResult<void> isEqual(const Expression &other) const;
};

[[nodiscard]] core::LogicalResult<
    core::Pair<core::UniquePtr<Program>, core::UniquePtr<symbolTable::Table>>>
parseFile(core::String filename);
[[nodiscard]] core::LogicalResult<
    core::Pair<core::UniquePtr<Program>, core::UniquePtr<symbolTable::Table>>>
parse(core::String content);

template <typename STREAM> STREAM &printTab(STREAM &stream, int tab) {
  for (int i = 0; i < tab; ++i)
    stream << "  ";
  return stream;
}

template <typename STREAM, typename T>
STREAM &prettyPrintBinExpr(STREAM &stream, int tab, T *expr, core::String op) {
  stream << "(";
  expr->lval->prettyPrint(stream, tab) << op;
  expr->rval->prettyPrint(stream, tab) << ")";
  return stream;
}

template <typename STREAM>
STREAM &Program::prettyPrint(STREAM &stream, int tab) const {
  bool first = true;
  for (auto &item : items) {
    if (!first)
      printTab(stream, tab);
    first = false;
    item->prettyPrint(stream, tab) << "\n";
  }
  return stream;
}
template <typename STREAM>
STREAM &FunctionDeclaration::prettyPrint(STREAM &stream, int tab) const {
  stream << "fun ";
  returnType->prettyPrint(stream, tab) << " " << name << "(";
  bool first = true;
  for (auto &arg : args) {
    if (!first)
      stream << ", ";
    first = false;
    arg->prettyPrint(stream, tab);
  }
  stream << ") ";
  body->prettyPrint(stream, tab);
  return stream;
}
template <typename STREAM>
STREAM &Argument::prettyPrint(STREAM &stream, int tab) const {
  type->prettyPrint(stream, tab) << " " << name;
  return stream;
}
template <typename STREAM>
STREAM &StructDeclaration::prettyPrint(STREAM &stream, int tab) const {
  stream << "struct " << name << " {\n";
  for (auto &f : fields) {
    printTab(stream, tab + 1);
    f->prettyPrint(stream, tab + 1) << "\n";
  }
  stream << "\n";
  for (auto &o : ops) {
    printTab(stream, tab + 1);
    o->prettyPrint(stream, tab + 1) << "\n";
  }
  stream << "\n";
  for (auto &m : methods) {
    printTab(stream, tab + 1);
    m->prettyPrint(stream, tab + 1) << "\n";
  }
  stream << "\n";
  for (auto &c : casts) {
    printTab(stream, tab + 1);
    c->prettyPrint(stream, tab + 1) << "\n";
  }
  printTab(stream, tab) << "}";
  return stream;
}
template <typename STREAM>
STREAM &UnionDeclaration::prettyPrint(STREAM &stream, int tab) const {
  stream << "union " << name << " {\n";
  printTab(stream, tab + 1);
  bool first = true;
  for (auto &f : fields) {
    if (!first)
      stream << ", ";
    first = false;
    f->prettyPrint(stream, tab);
  }
  stream << "\n";
  printTab(stream, tab);
  stream << "}";
  return stream;
}
template <typename STREAM>
STREAM &CastDeclaration::prettyPrint(STREAM &stream, int tab) const {
  stream << "cast ";
  type->prettyPrint(stream, tab) << " ";
  body->prettyPrint(stream, tab);
  return stream;
}

template <typename STREAM>
STREAM &OperationDeclaration::prettyPrint(STREAM &stream, int tab) const {
  stream << "operator " << to_string(opType) << "()";
  body->prettyPrint(stream, tab);
  return stream;
}
template <typename STREAM>
STREAM &VariableDeclaration::prettyPrint(STREAM &stream, int tab) const {
  stream << "var ";
  varType->prettyPrint(stream, tab);
  stream << " ";
  bool first = true;
  for (auto &d : decls) {
    if (!first)
      stream << ", ";
    first = false;
    d->prettyPrint(stream, tab);
  }
  stream << ";";
  return stream;
}
template <typename STREAM>
STREAM &Declarator::prettyPrint(STREAM &stream, int tab) const {
  stream << name;
  if (value != nullptr) {
    stream << " = ";
    value->prettyPrint(stream, tab);
  }
  return stream;
}
template <typename STREAM>
STREAM &CompoundStmt::prettyPrint(STREAM &stream, int tab) const {
  stream << "{\n";
  for (auto &item : items) {
    printTab(stream, tab + 1);
    item->prettyPrint(stream, tab + 1) << "\n";
  }
  printTab(stream, tab) << "}";
  return stream;
}
template <typename STREAM>
STREAM &WhileStmt::prettyPrint(STREAM &stream, int tab) const {
  stream << "while (";
  cond->prettyPrint(stream, tab);
  stream << ") ";
  body->prettyPrint(stream, tab);
  return stream;
}
template <typename STREAM>
STREAM &DoWhileStmt::prettyPrint(STREAM &stream, int tab) const {
  stream << "do ";
  body->prettyPrint(stream, tab);
  stream << "while (";
  cond->prettyPrint(stream, tab);
  stream << ")";
  return stream;
}
template <typename STREAM>
STREAM &ForStmt::prettyPrint(STREAM &stream, int tab) const {
  stream << "for (";
  if (init) {
    init->prettyPrint(stream, tab);
  } else {
    stream << ";";
  }
  if (cond)
    cond->prettyPrint(stream, tab);
  stream << ";";
  if (step)
    step->prettyPrint(stream, tab);
  stream << ") ";
  body->prettyPrint(stream, tab);
  return stream;
}
template <typename STREAM>
STREAM &IfStmt::prettyPrint(STREAM &stream, int tab) const {
  stream << "if (";
  cond->prettyPrint(stream, tab);
  stream << ") ";
  thenPart->prettyPrint(stream, tab);
  if (elsePart) {
    stream << " else ";
    elsePart->prettyPrint(stream, tab);
  }
  return stream;
}
template <typename STREAM>
STREAM &SwitchStmt::prettyPrint(STREAM &stream, int tab) const {
  stream << "switch (";
  cond->prettyPrint(stream, tab);
  stream << ") {\n";
  for (auto &item : items) {
    printTab(stream, tab + 1);
    item->prettyPrint(stream, tab + 1) << "\n";
  }
  printTab(stream, tab) << "}";
  return stream;
}
template <typename STREAM>
STREAM &SwitchCase::prettyPrint(STREAM &stream, int tab) const {
  stream << "case ";
  cond->prettyPrint(stream, tab);
  stream << ": ";
  body->prettyPrint(stream, tab);
  return stream;
}
template <typename STREAM>
STREAM &SwitchDefault::prettyPrint(STREAM &stream, int tab) const {
  stream << "default: ";
  body->prettyPrint(stream, tab);
  return stream;
}
template <typename STREAM>
STREAM &ExpressionStmt::prettyPrint(STREAM &stream, int tab) const {
  value->prettyPrint(stream, tab) << ";";
  return stream;
}

template <typename STREAM>
STREAM &NopStmt::prettyPrint(STREAM &stream, int) const {
  stream << ";";
  return stream;
}

template <typename STREAM>
STREAM &ContinueStmt::prettyPrint(STREAM &stream, int) const {
  stream << "continue";
  return stream;
}

template <typename STREAM>
STREAM &BreakStmt::prettyPrint(STREAM &stream, int) const {
  stream << "break";
  return stream;
}
template <typename STREAM>
STREAM &ReturnStmt::prettyPrint(STREAM &stream, int tab) const {
  if (value) {
    stream << "return ";
    value->prettyPrint(stream, tab);
    stream << ";";
  } else {
    stream << "return;";
  }
  return stream;
}
template <typename STREAM>
STREAM &PtrType::prettyPrint(STREAM &stream, int tab) const {
  base->prettyPrint(stream, tab) << "*";
  return stream;
}
template <typename STREAM>
STREAM &ArrayType::prettyPrint(STREAM &stream, int tab) const {
  base->prettyPrint(stream, tab) << "[";
  size->prettyPrint(stream, tab) << "]";
  return stream;
}
template <typename STREAM>
STREAM &FunType::prettyPrint(STREAM &stream, int tab) const {
  base->prettyPrint(stream, tab) << "(";
  bool first = true;
  for (auto &arg : args) {
    if (!first)
      stream << ", ";
    first = false;
    arg->prettyPrint(stream, tab);
  }
  stream << ")";
  return stream;
}

template <typename STREAM>
STREAM &BitType::prettyPrint(STREAM &stream, int) const {
  stream << "bit";
  return stream;
}

template <typename STREAM>
STREAM &VoidType::prettyPrint(STREAM &stream, int) const {
  stream << "void";
  return stream;
}

template <typename STREAM>
STREAM &TypelessType::prettyPrint(STREAM &stream, int) const {
  stream << "typeless";
  return stream;
}

template <typename STREAM>
STREAM &StructType::prettyPrint(STREAM &stream, int) const {
  stream << "struct " << name;
  return stream;
}

template <typename STREAM>
STREAM &UnionType::prettyPrint(STREAM &stream, int) const {
  stream << "union " << name;
  return stream;
}

template <typename STREAM>
STREAM &AffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " = ");
}
template <typename STREAM>
STREAM &AddAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " += ");
}
template <typename STREAM>
STREAM &SubAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " -= ");
}
template <typename STREAM>
STREAM &MulAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " *= ");
}
template <typename STREAM>
STREAM &ModAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " %= ");
}
template <typename STREAM>
STREAM &DivAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " /= ");
}
template <typename STREAM>
STREAM &LandAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " &&= ");
}
template <typename STREAM>
STREAM &LorAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " ||= ");
}
template <typename STREAM>
STREAM &LxorAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " ^^= ");
}
template <typename STREAM>
STREAM &BandAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " &= ");
}
template <typename STREAM>
STREAM &BorAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " |= ");
}
template <typename STREAM>
STREAM &BxorAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " ^= ");
}
template <typename STREAM>
STREAM &LshiftAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " <<= ");
}
template <typename STREAM>
STREAM &RshiftAffecExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " >>= ");
}
template <typename STREAM>
STREAM &LxorExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " ^^ ");
}
template <typename STREAM>
STREAM &LorExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " || ");
}
template <typename STREAM>
STREAM &LandExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " && ");
}
template <typename STREAM>
STREAM &BorExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " | ");
}
template <typename STREAM>
STREAM &BxorExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " ^ ");
}
template <typename STREAM>
STREAM &BandExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " & ");
}
template <typename STREAM>
STREAM &NeqExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " != ");
}
template <typename STREAM>
STREAM &EqExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " == ");
}
template <typename STREAM>
STREAM &LtExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " < ");
}
template <typename STREAM>
STREAM &LeExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " <= ");
}
template <typename STREAM>
STREAM &GtExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " > ");
}
template <typename STREAM>
STREAM &GeExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " >= ");
}
template <typename STREAM>
STREAM &LshiftExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " << ");
}
template <typename STREAM>
STREAM &RshiftExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " >> ");
}
template <typename STREAM>
STREAM &AddExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " + ");
}
template <typename STREAM>
STREAM &SubExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " - ");
}
template <typename STREAM>
STREAM &DivExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " / ");
}
template <typename STREAM>
STREAM &ModExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " % ");
}
template <typename STREAM>
STREAM &MulExpression::prettyPrint(STREAM &stream, int tab) const {
  return prettyPrintBinExpr(stream, tab, this, " * ");
}
template <typename STREAM>
STREAM &TernaryExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  cond->prettyPrint(stream, tab) << " ? ";
  thenPart->prettyPrint(stream, tab) << " : ";
  elsePart->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &PreIncrExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(++";
  base->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &PreDecrExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(--";
  base->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &PostIncrExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab);
  stream << "++)";
  return stream;
}
template <typename STREAM>
STREAM &PostDecrExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab);
  stream << "--)";
  return stream;
}
template <typename STREAM>
STREAM &DerefExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(*";
  base->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &AddrofExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(&";
  base->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &LnotExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(!";
  base->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &BnotExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(~";
  base->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &SyscallExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "syscall(";
  code->prettyPrint(stream, tab);
  for (auto &a : args) {
    stream << ", ";
    a->prettyPrint(stream, tab);
  }
  stream << ")";
  return stream;
}
template <typename STREAM>
STREAM &SizeofExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "sizeof(";
  arg->prettyPrint(stream, tab);
  stream << ")";
  return stream;
}
template <typename STREAM>
STREAM &ArrayExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab) << "[";
  index->prettyPrint(stream, tab) << "])";
  return stream;
}
template <typename STREAM>
STREAM &PtrFieldExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab) << "->" << name << ")";
  return stream;
}
template <typename STREAM>
STREAM &FieldExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab) << "." << name << ")";
  return stream;
}
template <typename STREAM>
STREAM &FunExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab) << "(";
  bool first = true;
  for (auto &a : args) {
    if (!first)
      stream << ", ";
    first = false;
    a->prettyPrint(stream, tab);
  }
  stream << "))";
  return stream;
}
template <typename STREAM>
STREAM &CastExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab) << " cast ";
  newType->prettyPrint(stream, tab) << ")";
  return stream;
}
template <typename STREAM>
STREAM &BitCastExpression::prettyPrint(STREAM &stream, int tab) const {
  stream << "(";
  base->prettyPrint(stream, tab) << " bitcast ";
  newType->prettyPrint(stream, tab) << ")";
  return stream;
}

template <typename STREAM>
STREAM &VarExpression::prettyPrint(STREAM &stream, int) const {
  stream << name;
  return stream;
}

template <typename STREAM>
STREAM &IntExpression::prettyPrint(STREAM &stream, int) const {
  stream << value;
  return stream;
}

template <typename STREAM>
STREAM &CharExpression::prettyPrint(STREAM &stream, int) const {
  stream << value;
  return stream;
}

template <typename STREAM>
STREAM &StringExpression::prettyPrint(STREAM &stream, int) const {
  stream << value;
  return stream;
}

template <typename STREAM>
STREAM &TrueExpression::prettyPrint(STREAM &stream, int) const {
  stream << "true";
  return stream;
}

template <typename STREAM>
STREAM &FalseExpression::prettyPrint(STREAM &stream, int) const {
  stream << "false";
  return stream;
}

template <typename STREAM>
STREAM &ThisExpression::prettyPrint(STREAM &stream, int) const {
  stream << "this";
  return stream;
}

template <typename STREAM>
STREAM &OtherExpression::prettyPrint(STREAM &stream, int) const {
  stream << "other";
  return stream;
}
template <typename STREAM>
STREAM &ProgramItem::prettyPrint(STREAM &stream, int tab) const {
  return this->visit(
      [&](auto &&elt) -> STREAM & { return elt.prettyPrint(stream, tab); });
}
template <typename STREAM>
STREAM &Type::prettyPrint(STREAM &stream, int tab) const {
  return this->visit(
      [&](auto &&elt) -> STREAM & { return elt.prettyPrint(stream, tab); });
}
template <typename STREAM>
STREAM &Statement::prettyPrint(STREAM &stream, int tab) const {
  return this->visit(
      [&](auto &&elt) -> STREAM & { return elt.prettyPrint(stream, tab); });
}
template <typename STREAM>
STREAM &SwitchItem::prettyPrint(STREAM &stream, int tab) const {
  return this->visit(
      [&](auto &&elt) -> STREAM & { return elt.prettyPrint(stream, tab); });
}
template <typename STREAM>
STREAM &Expression::prettyPrint(STREAM &stream, int tab) const {
  return this->visit(
      [&](auto &&elt) -> STREAM & { return elt.prettyPrint(stream, tab); });
}

}; // namespace ast