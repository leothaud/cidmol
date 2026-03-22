/* Copyright 2026 Dylan Leothaud
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *
 *
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
export module gssa:core;

import core;

namespace gssa {

export enum class OperationType {
  META_OP,
  GAMMA_OP,
  MU_OP,
  OR_OP,
  XOR_OP,
  AND_OP,
  CALL_OP,
  NOT_OP,
  ADDROF_OP,
  DEREF_OP,
  EXTRACT_OP,
  CONCAT_OP,
  SYSCALL_OP,
  LOAD_OP,
  STORE_OP,
  CONST_OP,
};

export struct Operation;
export struct Function;

export struct Value {
  u32 bitwidth, index;
  Operation *definingOperation;
  core::MultiSet<Operation *> uses;

  Value(u32 bitwidth, u32 index, Operation *definingOperation)
      : bitwidth(bitwidth), index(index), definingOperation(definingOperation) {
  }
  ~Value() { replaceWith(nullptr); }

  void replaceWith(Value *other) const;
  template <typename... ARGS>
    requires(core::isSame<ARGS, Operation *> && ...)
  void replaceWithExcept(Value *other, ARGS... exception);

  void removeUse(Operation *op) { uses.remove(op); }
  void removeUses(Operation *op) { uses.removeAll(op); }
};

export struct FunctionAttribute {
  Function *function;
};

export struct ValueListAttribute {
  core::Vector<core::UniquePtr<Value>> values;
};

export struct Attribute
    : public core::Variant<FunctionAttribute, ValueListAttribute> {
  using Variant::Variant;
};

export struct Operation {
  OperationType opType;
  core::Vector<u32> operandsWidths, resultsWidths;
  core::Vector<Value *> operands;
  core::Vector<core::UniquePtr<Value>> results;
  core::Map<core::String, core::UniquePtr<Attribute>> attributes;

protected:
  explicit Operation(OperationType opType) : opType(opType) {}

public:
  ~Operation() {
    for (auto *operand : operands) {
      operand->removeUse(this);
    }
  }

  void replaceOperand(const Value *from, Value *to) {
    for (u32 i = 0; i < operands.length(); ++i) {
      if (operands[i] == from)
        operands[i] = to;
    }
  }

  void replaceOperand(u32 index, Value *to) { operands[index] = to; }

  core::LogicalResult<void> verify() const;
};

export struct MetaOperation : public Operation {
  static constexpr char argAttrName[] = "arguments";
  MetaOperation(core::Vector<Value *> operands, core::Vector<u32> resultsWidths)
      : Operation(OperationType::META_OP) {
    ValueListAttribute args;
    for (u32 idx = 0; idx < operands.length(); ++idx) {
      auto *operand = operands[idx];
      operandsWidths.pushBack(operand->bitwidth);
      auto arg =
          core::UniquePtr<Value>::create(operand->bitwidth, idx, nullptr);
      args.values.pushBack(core::move(arg));
    }
    auto arguments = core::UniquePtr<Attribute>::create();
    *arguments = core::move(args);
    attributes.insert(argAttrName, core::move(arguments));

    for (u32 idx = 0; idx < resultsWidths.length(); ++idx) {
      results.pushBack(
          core::UniquePtr<Value>::create(resultsWidths[idx], idx, this));
    }
    this->resultsWidths = core::move(resultsWidths);
  }

  core::LogicalResult<void> verify() const {
    if (!attributes.contains(argAttrName)) {
      return core::LogicalResult<void>::failure(
          "MetaOperation does not have argument attribute.\n");
    }
    auto &argAttr = attributes.get(argAttrName);
    if (!argAttr->isA<ValueListAttribute>()) {
      return core::LogicalResult<void>::failure(
          "MetaOperation argument attribute should be a ValueListAttribute.\n");
    }
    auto &arguments = argAttr->get<ValueListAttribute>();
    if (arguments.values.length() != operandsWidths.length()) {
      return core::LogicalResult<void>::failure(
          "MetaOperation has the wrong number of arguments. Expecting " +
          core::String::of(operandsWidths.length()) + ", but got " +
          core::String::of(arguments.values.length()) + ".\n");
    }
    unsigned idx = 0;
    for (auto [bw, arg] : core::Zipped(operandsWidths, arguments.values)) {
      if (arg->bitwidth != bw) {
        return core::LogicalResult<void>::failure(
            "MetaOperation arguments of index " + core::String::of(idx) +
            " has the wrong bitwidth. Expecting " + core::String::of(bw) +
            ", but got " + core::String::of(arg->bitwidth) + ".\n");
      }
      ++idx;
    }
    return core::LogicalResult<void>::success();
  }
};

export struct GammaOperation : public Operation {
  Value *getCond() const { return operands[0]; }
  u32 getNumData() const { return operands.length() - 1; }
  Value *getData(u32 index) const { return operands[index + 1]; }

  core::LogicalResult<void> verify() const {
    u32 condBw = operands[0]->bitwidth;
    u32 minNumData = condBw == 1 ? 1 : ((1u << (condBw - 1)) + 1);
    u32 maxNumData = 1u << condBw;

    u32 numData = getNumData();

    if (numData < minNumData) {
      return core::LogicalResult<void>::failure(
          "GammaOperation condition has a bitwidth of " +
          core::String::of(condBw) + ", which is too much to select between " +
          core::String::of(numData) + " values.\n");
    }
    if (numData > maxNumData) {
      return core::LogicalResult<void>::failure(
          "GammaOperation condition has a bitwidth of " +
          core::String::of(condBw) +
          ", which is not enough to select between " +
          core::String::of(numData) + " values.\n");
    }

    u32 dataBw = operands[1]->bitwidth;
    for (u32 idx = 2; idx < operands.length(); ++idx) {
      if (operands[idx]->bitwidth != dataBw) {
        return core::LogicalResult<void>::failure(
            "All GammaOperation data should have same bitwidth. Expected " +
            core::String::of(dataBw) + ", but got " +
            core::String::of(operands[idx]->bitwidth) + " at index " +
            core::String::of(idx - 1) + ".\n");
      }
    }

    return core::LogicalResult<void>::success();
  }
};

export struct MuOperation : public Operation {
  static constexpr u32 initIndex = 0;
  static constexpr u32 loopIndex = 1;
  Value *getInit() const { return operands[initIndex]; }
  Value *getLoop() const { return operands[loopIndex]; }
  core::LogicalResult<void> verify() const {
    if (operands.length() != 2) {
      return core::LogicalResult<void>::failure(
          "MuOperation should always have 2 operands, but got " +
          core::String::of(operands.length()) + ".\n");
    }
    if (operands[initIndex]->bitwidth != operands[loopIndex]->bitwidth) {
      return core::LogicalResult<void>::failure(
          "MuOperation operands bitwidth should be the same, but got " +
          core::String::of(operands[initIndex]->bitwidth) + " for init and " +
          core::String::of(operands[loopIndex]->bitwidth) + " for loop.\n");
    }
    return core::LogicalResult<void>::success();
  }
};

export struct OrOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct XorOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct AndOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct CallOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct NotOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct AddrofOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct DerefOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct ExtractOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct ConcatOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct SyscallOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct LoadOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct StoreOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct ConstOperation : public Operation {
  core::LogicalResult<void> verify() const {
    return core::LogicalResult<void>::success();
  }
};

export struct Function {
  core::Vector<u32> argumentsWidths;
  core::Vector<core::UniquePtr<Value>> arguments;
  core::UniquePtr<MetaOperation> body;
  Value *result;
};

void Value::replaceWith(Value *other) const {
  for (auto *use : uses) {
    use->replaceOperand(this, other);
  }
}

template <typename... ARGS>
  requires(core::isSame<ARGS, Operation *> && ...)
void Value::replaceWithExcept(Value *other, ARGS... exception) {
  for (auto *use : uses) {
    if (!((use == exception) || ...)) {
      use->replaceOperand(this, other);
    }
  }
}

core::LogicalResult<void> Operation::verify() const {
  auto fail = [&] [[clang::always_inline]] (
                  core::String msg) -> core::LogicalResult<void> {
    return core::LogicalResult<void>::failure(msg);
  };
  if (operandsWidths.length() != operands.length()) {
    return fail("Operation has the wrong number of operands. Expecting " +
                core::String::of(operandsWidths.length()) + ", but got " +
                core::String::of(operands.length()) + ".\n");
  }
  for (u32 i = 0; i < operandsWidths.length(); ++i) {
    if (operands[i]->bitwidth != operandsWidths[i]) {
      return fail("Operation operand at index " + core::String::of(i) +
                  " has the wrong bit width. Expecting " +
                  core::String::of(operandsWidths[i]) + ", but got " +
                  core::String::of(operands[i]->bitwidth) + ".\n");
    }
  }
  if (resultsWidths.length() != results.length()) {
    return fail("Operation has the wrong number of results. Expecting " +
                core::String::of(resultsWidths.length()) + ", but got " +
                core::String::of(results.length()) + ".\n");
  }
  for (u32 i = 0; i < resultsWidths.length(); ++i) {
    if (results[i]->bitwidth != resultsWidths[i]) {
      return fail("Operation result at index " + core::String::of(i) +
                  " has the wrong bit width. Expecting " +
                  core::String::of(resultsWidths[i]) + ", but got " +
                  core::String::of(results[i]->bitwidth) + ".\n");
    }
  }

  switch (opType) {
  case OperationType::META_OP: {
    return static_cast<const MetaOperation *>(this)->verify();
  }
  case OperationType::GAMMA_OP: {
    return static_cast<const GammaOperation *>(this)->verify();
  }
  case OperationType::MU_OP: {
    return static_cast<const MuOperation *>(this)->verify();
  }
  case OperationType::OR_OP: {
    return static_cast<const OrOperation *>(this)->verify();
  }
  case OperationType::XOR_OP: {
    return static_cast<const XorOperation *>(this)->verify();
  }
  case OperationType::AND_OP: {
    return static_cast<const AndOperation *>(this)->verify();
  }
  case OperationType::CALL_OP: {
    return static_cast<const CallOperation *>(this)->verify();
  }
  case OperationType::NOT_OP: {
    return static_cast<const NotOperation *>(this)->verify();
  }
  case OperationType::ADDROF_OP: {
    return static_cast<const AddrofOperation *>(this)->verify();
  }
  case OperationType::DEREF_OP: {
    return static_cast<const DerefOperation *>(this)->verify();
  }
  case OperationType::EXTRACT_OP: {
    return static_cast<const ExtractOperation *>(this)->verify();
  }
  case OperationType::CONCAT_OP: {
    return static_cast<const ConcatOperation *>(this)->verify();
  }
  case OperationType::SYSCALL_OP: {
    return static_cast<const SyscallOperation *>(this)->verify();
  }
  case OperationType::LOAD_OP: {
    return static_cast<const LoadOperation *>(this)->verify();
  }
  case OperationType::STORE_OP: {
    return static_cast<const StoreOperation *>(this)->verify();
  }
  case OperationType::CONST_OP: {
    return static_cast<const ConstOperation *>(this)->verify();
  }
  default: {
    core::trap();
  }
  }
}

} // namespace gssa