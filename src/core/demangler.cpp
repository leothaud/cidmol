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
export module core:demangler;

import :string;
import :traits;
import :vector;
import :logicalResult;
import :zip;
import :map;
import :variant;
import :stringLiteral;
import :uniquePtr;
import :errors;

namespace core {

struct DemangledSymbol {
  enum type {
    BASE,
    BUILTIN,
    CHAIN,
    MODULE,
    MODULEBASE,
    SYMBOL,
    PTR,
    RVAL,
    LVAL,
    FUNTYPE,
    CONST,
    VOLATILE,
    TEMPLATE,
    STD,
    VIRTUAL,
    CONSTRUCTOR,
    DESTRUCTOR,
    LAMBDA,
    REQUIRE,
  };
  type t;
  DemangledSymbol(type t) : t(t) {}
  String toString(bool printConst = true) const;
  bool isVoidType() const;
  UniquePtr<DemangledSymbol> clone() const;
  bool hasTemplate() const;
  LogicalResult<String> getLastName() const;
  virtual ~DemangledSymbol() = default;
  String dump() const;
  bool isConstFun() const;
  bool isConstructor() const;
};

struct VirtualSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> base;
  enum virtualType { VTABLE, VTT, TYPEINFO, TYPEINFO_NAME, GUARD };
  virtualType type;
  VirtualSymbol(virtualType type, UniquePtr<DemangledSymbol> base)
      : DemangledSymbol(VIRTUAL), base(move(base)), type(type) {}
  String typeToString() const {
    switch (type) {
    case VTABLE:
      return "vtable for ";
    case VTT:
      return "VTT for ";
    case TYPEINFO:
      return "typeinfo for ";
    case TYPEINFO_NAME:
      return "typeinfo name for ";
    case GUARD:
      return "guard variable for ";
    default:
      trap();
    }
  }
  String toString(bool = true) const {
    return typeToString() + base->toString();
  }
  bool isVoidType() const { return false; }
  UniquePtr<DemangledSymbol> clone() const {
    return new VirtualSymbol(type, base->clone());
  }
  bool hasTemplate() const { return base->hasTemplate(); }
  String dump() const { return "VIRTUAL{" + base->dump() + "}"; }
  bool isConstFun() const { return false; }
};

struct ConstructorSymbol : public DemangledSymbol {
  ConstructorSymbol() : DemangledSymbol(CONSTRUCTOR) {}
  String dump() const { return "CONSTRUCTOR"; }
};

struct DestructorSymbol : public DemangledSymbol {
  DestructorSymbol() : DemangledSymbol(DESTRUCTOR) {}
  String dump() const { return "DESTRUCTOR"; }
};

struct StdSymbol : public DemangledSymbol {
  StdSymbol() : DemangledSymbol(STD) {}
  String toString(bool = true) const { return "std"; }
  StdSymbol *clone() const { return new StdSymbol; }
  bool hasTemplate() const { return false; }
  String dump() const { return "STD"; }
  bool isConstFun() const { return false; }
};

struct BaseSymbol : public DemangledSymbol {
  String name;
  bool constructor_;
  BaseSymbol(String name) : DemangledSymbol(BASE), name(name) {}
  String toString(bool = true) const { return name; }
  bool isVoidType() const { return false; }
  BaseSymbol *clone() const { return new BaseSymbol(name); }
  bool hasTemplate() const { return false; }
  LogicalResult<String> getLastName() const { return name; }
  String dump() const { return "BASE{" + name + "}"; }
  bool isConstFun() const { return false; }
  bool isConstructor() const { return constructor_; }
};

struct BuiltinSymbol : public DemangledSymbol {
  String name;
  bool constructor_;
  BuiltinSymbol(String name) : DemangledSymbol(BUILTIN), name(name) {}
  String toString(bool = true) const { return name; }
  bool isVoidType() const { return name == "void"; }
  BuiltinSymbol *clone() const { return new BuiltinSymbol(name); }
  bool hasTemplate() const { return false; }
  LogicalResult<String> getLastName() const { return name; }
  String dump() const { return "BUILTIN{" + name + "}"; }
  bool isConstFun() const { return false; }
  bool isConstructor() const { return constructor_; }
};

struct RequireSymbol : public DemangledSymbol {
  Vector<UniquePtr<DemangledSymbol>> conds;
  RequireSymbol() : DemangledSymbol(REQUIRE) {}
  void addCond(UniquePtr<DemangledSymbol> cond) { conds.pushBack(move(cond)); }
  String toString() const {
    return ""; // TODO: ???
  }
  RequireSymbol *clone() const {
    auto *res = new RequireSymbol;
    for (auto &c : conds) {
      res->addCond(c->clone());
    }
    return res;
  }
  bool hasTemplate() const { return false; }
  String dump() const {
    String result = "REQUIRES{";
    bool first = true;
    for (auto &c : conds) {
      if (!first)
        result += ", ";
      first = false;
      result += c->dump();
    }
    result += "}";
    return result;
  }
  bool isConstFun() const { return false; }
};

struct TemplateSymbol : public DemangledSymbol {
  Vector<UniquePtr<DemangledSymbol>> templates;
  UniquePtr<DemangledSymbol> rqs = nullptr;
  bool printLimits = true;
  TemplateSymbol() : DemangledSymbol(TEMPLATE) {}
  void addTemplate(UniquePtr<DemangledSymbol> t) {
    templates.pushBack(move(t));
  }
  String toString(bool = true) const {
    String result = printLimits ? "<" : "";
    bool first = true;
    for (auto &t : templates) {
      if (!first)
        result += ", ";
      first = false;
      result += t->toString();
    }
    return result + (printLimits ? ">" : "");
  }
  TemplateSymbol *clone() const {
    auto *res = new TemplateSymbol;
    for (auto &t : templates)
      res->addTemplate(t->clone());
    res->rqs = (rqs != nullptr) ? rqs->clone() : nullptr;
    return res;
  }
  bool hasTemplate() const { return true; }
  String dump() const {
    String result = "TEMPLATE{";
    bool first = true;
    for (auto &t : templates) {
      if (!first)
        result += ", ";
      first = false;
      result += t->dump();
    }
    if (rqs != nullptr) {
      result += ", RQS=" + rqs->dump();
    }
    result += "}";
    return result;
  }
  bool isConstFun() const { return false; }
};

struct ChainSymbol : public DemangledSymbol {
  Vector<UniquePtr<DemangledSymbol>> parts;
  ChainSymbol() : DemangledSymbol(CHAIN) {}
  void addPart(UniquePtr<DemangledSymbol> part) { parts.pushBack(move(part)); }
  String toString(bool printConst = true) const {
    String result = "";
    bool first = true;
    u64 size = parts.length();
    for (u64 idx = 0; idx < size; ++idx) {
      auto &p = parts[idx];
      if (!first && (p->t != TEMPLATE))
        result += "::";
      first = false;
      result += p->toString((idx == size - 1) ? printConst : true);
    }
    return result;
  }
  ChainSymbol *clone() const {
    auto *res = new ChainSymbol;
    for (auto &p : parts)
      res->addPart(p->clone());
    return res;
  }
  bool hasTemplate() const {
    assert(parts.length() > 0);
    return parts[parts.length() - 1]->hasTemplate();
  }
  LogicalResult<String> getLastName() const {
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
      auto name = (*it)->getLastName();
      if (!name.failed())
        return name;
    }
    return LogicalResult<String>::failure("No name found.");
  }
  String dump() const {
    String result = "CHAIN{";
    bool first = true;
    for (auto &p : parts) {
      if (!first)
        result += ", ";
      first = false;
      result += p->dump();
    }
    result += "}";
    return result;
  }
  bool isConstFun() const {
    if (parts.length() == 0)
      return false;
    return parts[parts.length() - 1]->isConstFun();
  }
  bool isConstructor() const {
    if (parts.length() == 0)
      return false;
    return parts[parts.length() - 1]->isConstructor();
  }
};

struct ModuleBaseSymbol : public DemangledSymbol {
  String name;
  ModuleBaseSymbol(String name) : DemangledSymbol(MODULEBASE), name(name) {}
  String toString(bool = true) const { return name; }
  ModuleBaseSymbol *clone() const { return new ModuleBaseSymbol(name); }
  bool hasTemplate() const { return false; }
  String dump() const { return "MODULEBASE{" + name + "}"; }
};

struct ModuleSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> inner, base;
  ModuleSymbol(UniquePtr<DemangledSymbol> inner,
               UniquePtr<DemangledSymbol> base)
      : DemangledSymbol(MODULE), inner(move(inner)), base(move(base)) {}
  String toString(bool printConst = true) const {
    return inner->toString(printConst) + "@" + base->toString();
  }
  ModuleSymbol *clone() const {
    return new ModuleSymbol(inner->clone(), base->clone());
  }
  bool hasTemplate() const { return inner->hasTemplate(); }
  LogicalResult<String> getLastName() const { return inner->getLastName(); }
  String dump() const {
    return "MODULE{" + inner->dump() + ", " + base->dump() + "}";
  }
  bool isConstFun() const { return inner->isConstFun(); }
  bool isConstructor() const { return inner->isConstructor(); }
};

struct Symbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> name, returnType, templates;
  Vector<UniquePtr<DemangledSymbol>> types;
  bool hasType = false, isConst = false, hasTemplate_ = false,
       hasReturnType = false;
  Symbol(UniquePtr<DemangledSymbol> name)
      : DemangledSymbol(SYMBOL), name(move(name)) {}
  void addType(UniquePtr<DemangledSymbol> type) { types.pushBack(move(type)); }
  String toString(bool printConst = true) const;
  Symbol *clone() const {
    auto *res = new Symbol(name->clone());
    res->isConst = isConst;
    res->hasType = hasType;
    res->hasReturnType = hasReturnType;
    res->hasTemplate_ = hasTemplate_;
    if (hasReturnType)
      res->returnType = returnType->clone();
    if (hasTemplate_)
      res->templates = templates->clone();
    for (auto &t : types)
      res->addType(t->clone());
    return res;
  }
  bool hasTemplate() const { return hasTemplate_ || name->hasTemplate(); }
  LogicalResult<String> getLastName() const { return name->getLastName(); }

  String dump() const {
    String result = "SYMBOL{";
    result += name->dump();
    if (hasReturnType) {
      result += ", returnType=" + returnType->dump();
    }
    if (hasTemplate_) {
      result += ", template=" + templates->dump();
    }
    if (isConst) {
      result += ", CONST";
    }
    if (hasType) {
      result += ", TYPES";
    }
    for (auto &t : types) {
      result += t->dump();
    }
    return result + "}";
  }
  bool isConstFun() const { return isConst || name->isConstFun(); }
  bool isConstructor() const { return name->isConstructor(); }
};

struct PtrSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> inner;
  int depth = 1;
  PtrSymbol(UniquePtr<DemangledSymbol> inner)
      : DemangledSymbol(PTR), inner(move(inner)) {}
  String toString(bool printConst = true) const;
  PtrSymbol *clone() const {
    auto *res = new PtrSymbol(inner->clone());
    res->depth = depth;
    return res;
  }
  bool hasTemplate() const { return false; }
  String dump() const {
    return "PTR{" + inner->dump() + ", " + String::of(depth) + "}";
  }
  bool isConstFun() const { return false; }
};

struct RvalSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> inner;
  RvalSymbol(UniquePtr<DemangledSymbol> inner)
      : DemangledSymbol(RVAL), inner(move(inner)) {}
  String toString(bool printConst = true) const;
  RvalSymbol *clone() const { return new RvalSymbol(inner->clone()); }
  bool hasTemplate() const { return false; }
  String dump() const { return "RVAL{" + inner->dump() + "}"; }
  bool isConstFun() const { return false; }
};

struct LvalSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> inner;
  LvalSymbol(UniquePtr<DemangledSymbol> inner)
      : DemangledSymbol(LVAL), inner(move(inner)) {}
  String toString(bool printConst = true) const;
  LvalSymbol *clone() const { return new LvalSymbol(inner->clone()); }
  bool hasTemplate() const { return false; }
  String dump() const { return "LVAL{" + inner->dump() + "}"; }
  bool isConstFun() const { return false; }
};

struct FunTypeSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> returnType;
  Vector<UniquePtr<DemangledSymbol>> arguments;

  FunTypeSymbol(UniquePtr<DemangledSymbol> returnType)
      : DemangledSymbol(FUNTYPE), returnType(move(returnType)) {}
  void addArgument(UniquePtr<DemangledSymbol> argType) {
    arguments.pushBack(move(argType));
  }
  String toString(bool = true) const {
    String result = returnType->toString();
    if ((arguments.length() == 1) && arguments[0]->isVoidType()) {
      result += "()";
    } else {
      result += "(";
      bool first = true;
      for (auto &a : arguments) {
        if (!first)
          result += ", ";
        first = false;
        result += a->toString();
      }
      result += ")";
    }
    return result;
  }
  FunTypeSymbol *clone() const {
    auto *res = new FunTypeSymbol(returnType->clone());
    for (auto &a : arguments)
      res->addArgument(a->clone());
    return res;
  }
  bool hasTemplate() const { return false; }
  String dump() const {
    String result = "FUNTYPE{" + returnType->dump();
    for (auto &t : arguments) {
      result += ", " + t->dump();
    }
    return result + "}";
  }
  bool isConstFun() const { return false; }
};

struct ConstSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> inner;
  ConstSymbol(UniquePtr<DemangledSymbol> inner)
      : DemangledSymbol(CONST), inner(move(inner)) {}
  String toString(bool printConst = true) const {
    if (printConst)
      return inner->toString() + " const";
    return inner->toString(printConst);
  }
  ConstSymbol *clone() const { return new ConstSymbol(inner->clone()); }
  bool hasTemplate() const { return inner->hasTemplate(); }
  String dump() const { return "CONST{" + inner->dump() + "}"; }
  bool isConstFun() const { return true; }
  bool isConstructor() const { return inner->isConstructor(); }
};
struct VolatileSymbol : public DemangledSymbol {
  UniquePtr<DemangledSymbol> inner;
  VolatileSymbol(UniquePtr<DemangledSymbol> inner)
      : DemangledSymbol(VOLATILE), inner(move(inner)) {}
  String toString(bool printConst = true) const {
    return inner->toString(printConst) + " volatile";
  }
  VolatileSymbol *clone() const { return new VolatileSymbol(inner->clone()); }
  bool hasTemplate() const { return inner->hasTemplate(); }
  LogicalResult<String> getLastName() const { return inner->getLastName(); }
  String dump() const { return "VOLATILE{" + inner->dump() + "}"; }
  bool isConstFun() const { return inner->isConstFun(); }
  bool isConstructor() const { return inner->isConstructor(); }
};

struct LambdaSymbol : public DemangledSymbol {
  Vector<UniquePtr<DemangledSymbol>> types;
  u64 index = 1;
  LambdaSymbol() : DemangledSymbol(LAMBDA) {}
  void setIndex(u64 index) { this->index = index; }
  void addType(UniquePtr<DemangledSymbol> t) { types.pushBack(move(t)); }
  String toString(bool = true) const {
    if ((types.length() == 1) && types[0]->isVoidType()) {
      return "{lambda()#" + String::of(index) + "}";
    }
    String result = "{lambda(";
    bool first = true;
    for (auto &t : types) {
      if (!first)
        result += ", ";
      first = false;
      result += t->toString();
    }
    result += ")#" + String::of(index) + "}";
    return result;
  }
  LambdaSymbol *clone() const {
    auto *result = new LambdaSymbol;
    result->index = index;
    for (auto &t : types) {
      result->types.pushBack(t->clone());
    }
    return result;
  }
  bool hasTemplate() const { return false; }
  String dump() const {
    String result = "LAMBDA{";
    bool first = true;
    for (auto &t : types) {
      if (!first)
        result += ", ";
      first = false;
      result += t->dump();
    }
    return result + ", " + String::of(index) + "}";
  }
  bool isConstFun() const { return false; }
};

LogicalResult<String> DemangledSymbol::getLastName() const {
  switch (t) {
  case BASE:
    return static_cast<const BaseSymbol *>(this)->getLastName();
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->getLastName();
  case CHAIN:
    return static_cast<const ChainSymbol *>(this)->getLastName();
  case MODULE:
    return static_cast<const ModuleSymbol *>(this)->getLastName();
  case SYMBOL:
    return static_cast<const Symbol *>(this)->getLastName();
  case VOLATILE:
    return static_cast<const VolatileSymbol *>(this)->getLastName();
  default:
    return LogicalResult<String>::failure("No name found.");
  }
}

bool DemangledSymbol::isConstFun() const {
  switch (t) {
  case VIRTUAL:
    return static_cast<const VirtualSymbol *>(this)->isConstFun();
  case STD:
    return static_cast<const StdSymbol *>(this)->isConstFun();
  case BASE:
    return static_cast<const BaseSymbol *>(this)->isConstFun();
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->isConstFun();
  case TEMPLATE:
    return static_cast<const TemplateSymbol *>(this)->isConstFun();
  case REQUIRE:
    return static_cast<const RequireSymbol *>(this)->isConstFun();
  case CHAIN:
    return static_cast<const ChainSymbol *>(this)->isConstFun();
  case MODULE:
    return static_cast<const ModuleSymbol *>(this)->isConstFun();
  case SYMBOL:
    return static_cast<const Symbol *>(this)->isConstFun();
  case PTR:
    return static_cast<const PtrSymbol *>(this)->isConstFun();
  case RVAL:
    return static_cast<const RvalSymbol *>(this)->isConstFun();
  case LVAL:
    return static_cast<const LvalSymbol *>(this)->isConstFun();
  case FUNTYPE:
    return static_cast<const FunTypeSymbol *>(this)->isConstFun();
  case VOLATILE:
    return static_cast<const VolatileSymbol *>(this)->isConstFun();
  case LAMBDA:
    return static_cast<const LambdaSymbol *>(this)->isConstFun();
  case CONST:
    return static_cast<const ConstSymbol *>(this)->isConstFun();
  default:
    trap();
  }
}

bool DemangledSymbol::hasTemplate() const {
  switch (t) {
  case BASE:
    return static_cast<const BaseSymbol *>(this)->hasTemplate();
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->hasTemplate();
  case CHAIN:
    return static_cast<const ChainSymbol *>(this)->hasTemplate();
  case MODULE:
    return static_cast<const ModuleSymbol *>(this)->hasTemplate();
  case MODULEBASE:
    return static_cast<const ModuleBaseSymbol *>(this)->hasTemplate();
  case SYMBOL:
    return static_cast<const Symbol *>(this)->hasTemplate();
  case PTR:
    return static_cast<const PtrSymbol *>(this)->hasTemplate();
  case RVAL:
    return static_cast<const RvalSymbol *>(this)->hasTemplate();
  case LVAL:
    return static_cast<const LvalSymbol *>(this)->hasTemplate();
  case FUNTYPE:
    return static_cast<const FunTypeSymbol *>(this)->hasTemplate();
  case CONST:
    return static_cast<const ConstSymbol *>(this)->hasTemplate();
  case VOLATILE:
    return static_cast<const VolatileSymbol *>(this)->hasTemplate();
  case TEMPLATE:
    return static_cast<const TemplateSymbol *>(this)->hasTemplate();
  case REQUIRE:
    return static_cast<const RequireSymbol *>(this)->hasTemplate();
  case STD:
    return static_cast<const StdSymbol *>(this)->hasTemplate();
  case VIRTUAL:
    return static_cast<const VirtualSymbol *>(this)->hasTemplate();
  case LAMBDA:
    return static_cast<const LambdaSymbol *>(this)->hasTemplate();
  default:
    trap();
  }
}

UniquePtr<DemangledSymbol> DemangledSymbol::clone() const {
  switch (t) {
  case BASE:
    return static_cast<const BaseSymbol *>(this)->clone();
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->clone();
  case CHAIN:
    return static_cast<const ChainSymbol *>(this)->clone();
  case MODULE:
    return static_cast<const ModuleSymbol *>(this)->clone();
  case MODULEBASE:
    return static_cast<const ModuleBaseSymbol *>(this)->clone();
  case SYMBOL:
    return static_cast<const Symbol *>(this)->clone();
  case PTR:
    return static_cast<const PtrSymbol *>(this)->clone();
  case RVAL:
    return static_cast<const RvalSymbol *>(this)->clone();
  case LVAL:
    return static_cast<const LvalSymbol *>(this)->clone();
  case FUNTYPE:
    return static_cast<const FunTypeSymbol *>(this)->clone();
  case CONST:
    return static_cast<const ConstSymbol *>(this)->clone();
  case VOLATILE:
    return static_cast<const VolatileSymbol *>(this)->clone();
  case TEMPLATE:
    return static_cast<const TemplateSymbol *>(this)->clone();
  case REQUIRE:
    return static_cast<const RequireSymbol *>(this)->clone();
  case STD:
    return static_cast<const StdSymbol *>(this)->clone();
  case VIRTUAL:
    return static_cast<const VirtualSymbol *>(this)->clone();
  case LAMBDA:
    return static_cast<const LambdaSymbol *>(this)->clone();
  default:
    trap();
  }
}

String PtrSymbol::toString(bool) const {
  if (inner->t == FUNTYPE) {
    auto *fun = static_cast<const FunTypeSymbol *>(inner.getRaw());
    String result = fun->returnType->toString();
    result += " (";
    for (int i = 0; i < depth; ++i)
      result += "*";
    if ((fun->arguments.length() == 1) && fun->arguments[0]->isVoidType()) {
      result += ") ()";
      return result;
    } else {
      result += ") (";
      bool first = true;
      for (auto &a : fun->arguments) {
        if (!first)
          result += ", ";
        first = false;
        result += a->toString();
      }
      result += ")";
      return result;
    }
  } else {
    String result = inner->toString();
    for (int i = 0; i < depth; ++i)
      result += "*";
    return result;
  }
}

String LvalSymbol::toString(bool) const {
  switch (inner->t) {
  case FUNTYPE: {
    auto *fun = static_cast<const FunTypeSymbol *>(inner.getRaw());
    String result = fun->returnType->toString();
    if ((fun->arguments.length() == 1) && fun->arguments[0]->isVoidType()) {
      result += "(&) ()";
      return result;
    } else {
      result += " (&) (";
      bool first = true;
      for (auto &a : fun->arguments) {
        if (!first)
          result += ", ";
        first = false;
        result += a->toString();
      }
      result += ")";
      return result;
    }
  }
  case LVAL: {
    return inner->toString();
  }
  case RVAL: {
    auto *rval = static_cast<const RvalSymbol *>(inner.getRaw());
    return rval->inner->toString() + "&";
  }
  default: {
    return inner->toString() + "&";
  }
  }
}

String RvalSymbol::toString(bool) const {
  switch (inner->t) {
  case FUNTYPE: {
    auto *fun = static_cast<const FunTypeSymbol *>(inner.getRaw());
    String result = fun->returnType->toString();
    if ((fun->arguments.length() == 1) && fun->arguments[0]->isVoidType()) {
      result += "(&&) ()";
      return result;
    } else {
      result += " (&&) (";
      bool first = true;
      for (auto &a : fun->arguments) {
        if (!first)
          result += ", ";
        first = false;
        result += a->toString();
      }
      result += ")";
      return result;
    }
  }
  default: {
    auto innerStr = inner->toString();
    if (innerStr.endsWith('&'))
      return innerStr;
    return innerStr + "&&";
  }
  }
}

String DemangledSymbol::toString(bool printConst) const {
  switch (t) {
  case BASE:
    return static_cast<const BaseSymbol *>(this)->toString(printConst);
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->toString(printConst);
  case CHAIN:
    return static_cast<const ChainSymbol *>(this)->toString(printConst);
  case MODULE:
    return static_cast<const ModuleSymbol *>(this)->toString(printConst);
  case MODULEBASE:
    return static_cast<const ModuleBaseSymbol *>(this)->toString(printConst);
  case SYMBOL:
    return static_cast<const Symbol *>(this)->toString(printConst);
  case PTR:
    return static_cast<const PtrSymbol *>(this)->toString(printConst);
  case RVAL:
    return static_cast<const RvalSymbol *>(this)->toString(printConst);
  case LVAL:
    return static_cast<const LvalSymbol *>(this)->toString(printConst);
  case FUNTYPE:
    return static_cast<const FunTypeSymbol *>(this)->toString(printConst);
  case CONST:
    return static_cast<const ConstSymbol *>(this)->toString(printConst);
  case VOLATILE:
    return static_cast<const VolatileSymbol *>(this)->toString(printConst);
  case TEMPLATE:
    return static_cast<const TemplateSymbol *>(this)->toString(printConst);
  case REQUIRE:
    return static_cast<const RequireSymbol *>(this)->toString();
  case STD:
    return static_cast<const StdSymbol *>(this)->toString(printConst);
  case VIRTUAL:
    return static_cast<const VirtualSymbol *>(this)->toString(printConst);
  case LAMBDA:
    return static_cast<const LambdaSymbol *>(this)->toString(printConst);
  default:
    trap();
  }
}

String DemangledSymbol::dump() const {
  switch (t) {
  case BASE:
    return static_cast<const BaseSymbol *>(this)->dump();
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->dump();
  case CHAIN:
    return static_cast<const ChainSymbol *>(this)->dump();
  case MODULE:
    return static_cast<const ModuleSymbol *>(this)->dump();
  case MODULEBASE:
    return static_cast<const ModuleBaseSymbol *>(this)->dump();
  case SYMBOL:
    return static_cast<const Symbol *>(this)->dump();
  case PTR:
    return static_cast<const PtrSymbol *>(this)->dump();
  case RVAL:
    return static_cast<const RvalSymbol *>(this)->dump();
  case LVAL:
    return static_cast<const LvalSymbol *>(this)->dump();
  case FUNTYPE:
    return static_cast<const FunTypeSymbol *>(this)->dump();
  case CONST:
    return static_cast<const ConstSymbol *>(this)->dump();
  case VOLATILE:
    return static_cast<const VolatileSymbol *>(this)->dump();
  case TEMPLATE:
    return static_cast<const TemplateSymbol *>(this)->dump();
  case REQUIRE:
    return static_cast<const RequireSymbol *>(this)->dump();
  case STD:
    return static_cast<const StdSymbol *>(this)->dump();
  case VIRTUAL:
    return static_cast<const VirtualSymbol *>(this)->dump();
  case LAMBDA:
    return static_cast<const LambdaSymbol *>(this)->dump();
  default:
    trap();
  }
}

bool DemangledSymbol::isConstructor() const {
  switch (t) {
  case BASE: {
    return static_cast<const BaseSymbol *>(this)->isConstructor();
  }
  case BUILTIN: {
    return static_cast<const BuiltinSymbol *>(this)->isConstructor();
  }
  case CHAIN: {
    return static_cast<const ChainSymbol *>(this)->isConstructor();
  }
  case MODULE: {
    return static_cast<const ModuleSymbol *>(this)->isConstructor();
  }
  case SYMBOL: {
    return static_cast<const Symbol *>(this)->isConstructor();
  }
  case CONST: {
    return static_cast<const ConstSymbol *>(this)->isConstructor();
  }
  case VOLATILE: {
    return static_cast<const VolatileSymbol *>(this)->isConstructor();
  }
  default: {
    return false;
  }
  }
}

bool DemangledSymbol::isVoidType() const {
  switch (t) {
  case BUILTIN:
    return static_cast<const BuiltinSymbol *>(this)->isVoidType();
  default: {
    return false;
  }
  }
}
String Symbol::toString(bool) const {
  String result = hasReturnType ? (returnType->toString() + " ") : "";
  bool isConst = this->isConstFun();
  result += name->toString(!isConst);
  if (hasTemplate_) {
    result += templates->toString();
  }
  if (hasType) {
    if ((types.length() == 1) && (types[0]->isVoidType())) {
      result += "()";
    } else {
      result += "(";
      bool first = true;
      for (auto &t : types) {
        if (!first)
          result += ", ";
        first = false;
        result += t->toString();
      }
      result += ")";
    }
  }
  if (isConst) {
    result += " const";
  }
  return result;
}

void parseSubstitutionIndex(u64 &size, const char *(&ptr)) {
  size = 0;
  auto toInt = [](char value) {
    if ((value >= '0') && (value <= '9'))
      return value - '0';
    return value - 'A' + 10;
  };
  bool first = true;
  while (*ptr != '_') {
    first = false;
    size = size * 36 + toInt(*ptr);
    ++ptr;
  }
  if (!first)
    ++size;
  ++ptr;
}

void parseSize(u64 &size, const char *(&ptr)) {
  size = 0;
  while ((*ptr >= '0') && (*ptr <= '9')) {
    size = size * 10 + (*ptr - '0');
    ++ptr;
  }
}

template <bool END_ON_E = false>
LogicalResult<UniquePtr<DemangledSymbol>>
demangle(const char *(&ptr), Vector<UniquePtr<DemangledSymbol>> &substitutions,
         Vector<UniquePtr<DemangledSymbol>> &templates);
LogicalResult<UniquePtr<DemangledSymbol>>
demangleModule(const char *(&ptr),
               Vector<UniquePtr<DemangledSymbol>> &substitutions,
               Vector<UniquePtr<DemangledSymbol>> &templates);
LogicalResult<UniquePtr<DemangledSymbol>> demangleName(
    const char *(&ptr), Vector<UniquePtr<DemangledSymbol>> &substitutions,
    Vector<UniquePtr<DemangledSymbol>> &templates, bool &isSubstitution);
LogicalResult<UniquePtr<DemangledSymbol>> demangleType(
    const char *(&ptr), Vector<UniquePtr<DemangledSymbol>> &substitutions,
    Vector<UniquePtr<DemangledSymbol>> &templates, bool &isSubstitution,
    Vector<UniquePtr<DemangledSymbol>> *currentTemplates = nullptr);
LogicalResult<UniquePtr<DemangledSymbol>>
demangleTemplate(const char *(&ptr),
                 Vector<UniquePtr<DemangledSymbol>> &substitutions,
                 Vector<UniquePtr<DemangledSymbol>> &templates);
template <bool isInRequire = false>
LogicalResult<UniquePtr<DemangledSymbol>>
demangleExpression(const char *(&ptr),
                   Vector<UniquePtr<DemangledSymbol>> &substitutions,
                   Vector<UniquePtr<DemangledSymbol>> &templates);

LogicalResult<UniquePtr<DemangledSymbol>>
demangleNestedName(const char *(&ptr),
                   Vector<UniquePtr<DemangledSymbol>> &substitutions,
                   Vector<UniquePtr<DemangledSymbol>> &templates) {
  if (*ptr == 'K') {
    auto inner = demangleNestedName(++ptr, substitutions, templates);
    if (inner.failed())
      return inner;
    return new ConstSymbol(move(*inner));
  }
  auto *result = new ChainSymbol;
  bool wasSubstitution = true;
  while (*ptr != 'E') {
    if (!wasSubstitution)
      substitutions.pushBack(result->clone());
    wasSubstitution = false;
    if (*ptr == 'W') {
      auto mod = demangleModule(++ptr, substitutions, templates);
      if (mod.failed()) {
        delete result;
        return mod;
      }
      result->addPart(move(*mod));
    } else {
      auto demangledName =
          demangleName(ptr, substitutions, templates, wasSubstitution);
      if (demangledName.failed()) {
        delete result;
        return demangledName;
      }
      switch ((*demangledName)->t) {
      case DemangledSymbol::CONSTRUCTOR: {
        auto lastName = result->getLastName();
        if (lastName.failed())
          return lastName;
        auto *newPart = new BaseSymbol(*lastName);
        newPart->constructor_ = true;
        result->addPart(newPart);
        break;
      }
      case DemangledSymbol::DESTRUCTOR: {
        auto lastName = result->getLastName();
        if (lastName.failed())
          return lastName;
        auto *newPart = new BaseSymbol("~" + *lastName);
        result->addPart(newPart);
        break;
      }
      case DemangledSymbol::TEMPLATE: {
        auto index = result->parts.length() - 1;
        auto symb = new Symbol(move(result->parts[index]));
        symb->hasTemplate_ = true;
        symb->templates = move(*demangledName);
        result->parts[index] = symb;
        break;
      }
      default: {
        result->addPart(move(*demangledName));
      }
      }
    }
  }
  ++ptr;
  return result;
}

extern "C" [[gnu::used]] void
printSubstitution(Vector<UniquePtr<DemangledSymbol>> &substitutions) {
  core::sout << "size: " << substitutions.length() << "\n";
  for (u64 i = 0; i < substitutions.length(); ++i) {
    core::sout << i << " -> " << substitutions[i]->toString();
    if (substitutions[i]->t == DemangledSymbol::MODULEBASE) {
      core::sout << " (module)";
    }
    core::sout << "\n";
  }
}

LogicalResult<UniquePtr<DemangledSymbol>> demangleName(
    const char *(&ptr), Vector<UniquePtr<DemangledSymbol>> &substitutions,
    Vector<UniquePtr<DemangledSymbol>> &templates, bool &isSubstitution) {
  switch (*ptr) {
  case 'a': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'a':
      ++ptr;
      return new BuiltinSymbol("operator&&");
    case 'd':
      ++ptr;
      return new BuiltinSymbol("operator&");
    case 'n':
      ++ptr;
      return new BuiltinSymbol("operator&");
    case 'N':
      ++ptr;
      return new BuiltinSymbol("operator&=");
    case 'S':
      ++ptr;
      return new BuiltinSymbol("operator=");
    case 'w':
      ++ptr;
      return new BuiltinSymbol("operatorco_await");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'c': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'l':
      ++ptr;
      return new BuiltinSymbol("operator()");
    case 'm':
      ++ptr;
      return new BuiltinSymbol("operator,");
    case 'o':
      ++ptr;
      return new BuiltinSymbol("operator~");
    case 'v': {
      ++ptr;
      bool useless;
      auto to = demangleType(ptr, substitutions, templates, useless);
      if (to.failed())
        return to;
      auto *res = new BuiltinSymbol("operator" + (*to)->toString());
      return res;
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'd': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'a':
      ++ptr;
      return new BuiltinSymbol("operatordelete[]");
    case 'e':
      ++ptr;
      return new BuiltinSymbol("operator*");
    case 'l':
      ++ptr;
      return new BuiltinSymbol("operatordelete");
    case 'v':
      ++ptr;
      return new BuiltinSymbol("operator/");
    case 'V':
      ++ptr;
      return new BuiltinSymbol("operator/=");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'e': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'o':
      ++ptr;
      return new BuiltinSymbol("operator^");
    case 'O':
      ++ptr;
      return new BuiltinSymbol("operator^=");
    case 'q':
      ++ptr;
      return new BuiltinSymbol("operator==");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'g': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'e':
      ++ptr;
      return new BuiltinSymbol("operator>=");
    case 't':
      ++ptr;
      return new BuiltinSymbol("operator>");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'i': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'x':
      ++ptr;
      return new BuiltinSymbol("operator[]");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'l': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'e':
      ++ptr;
      return new BuiltinSymbol("operator<=");
    case 'i': {
      ++ptr;
      u64 size;
      parseSize(size, ptr);
      String op = "\"\"";
      for (u64 i = 0; i < size; ++i, ++ptr) {
        op += *ptr;
      }
      return new BuiltinSymbol("operator" + op);
    }
    case 's':
      ++ptr;
      return new BuiltinSymbol("operator<<");
    case 'S':
      ++ptr;
      return new BuiltinSymbol("operator<<=");
    case 't':
      ++ptr;
      return new BuiltinSymbol("operator<");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'm': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'i':
      ++ptr;
      return new BuiltinSymbol("operator-");
    case 'I':
      ++ptr;
      return new BuiltinSymbol("operator-=");
    case 'l':
      ++ptr;
      return new BuiltinSymbol("operator*");
    case 'L':
      ++ptr;
      return new BuiltinSymbol("operator*=");
    case 'm':
      ++ptr;
      return new BuiltinSymbol("operator--");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'n': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'a':
      ++ptr;
      return new BuiltinSymbol("operatornew[]");
    case 'e':
      ++ptr;
      return new BuiltinSymbol("operator!=");
    case 'g':
      ++ptr;
      return new BuiltinSymbol("operator-");
    case 't':
      ++ptr;
      return new BuiltinSymbol("operator!");
    case 'w':
      ++ptr;
      return new BuiltinSymbol("operatornew");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'o': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'o':
      ++ptr;
      return new BuiltinSymbol("operator||");
    case 'r':
      ++ptr;
      return new BuiltinSymbol("operator|");
    case 'R':
      ++ptr;
      return new BuiltinSymbol("operator|=");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'p': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'l':
      ++ptr;
      return new BuiltinSymbol("operator+");
    case 'L':
      ++ptr;
      return new BuiltinSymbol("operator+=");
    case 'm':
      ++ptr;
      return new BuiltinSymbol("operator->*");
    case 'p':
      ++ptr;
      return new BuiltinSymbol("operator++");
    case 's':
      ++ptr;
      return new BuiltinSymbol("operator+");
    case 't':
      ++ptr;
      return new BuiltinSymbol("operator->");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'q': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'u':
      ++ptr;
      return new BuiltinSymbol("operator?");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'r': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'm':
      ++ptr;
      return new BuiltinSymbol("operator%");
    case 'M':
      ++ptr;
      return new BuiltinSymbol("operator%=");
    case 's':
      ++ptr;
      return new BuiltinSymbol("operator>>");
    case 'S':
      ++ptr;
      return new BuiltinSymbol("operator>>=");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 's': {
    const char *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 's':
      ++ptr;
      return new BuiltinSymbol("operator<=>");
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
    break;
  }
  case 'W': {
    return demangleModule(++ptr, substitutions, templates);
  }
  case 'S': {
    ++ptr;
    if (*ptr == 't') {
      ++ptr;
      return new StdSymbol;
    } else {
      u64 index;
      parseSubstitutionIndex(index, ptr);
      assert(substitutions.length() > index);
      auto entry = substitutions[index]->clone();
      if (entry->t == DemangledSymbol::MODULEBASE) {
        bool useless;
        auto next = demangleName(ptr, substitutions, templates, useless);
        if (next.failed())
          return next;
        auto *res = new ModuleSymbol(move(*next), move(entry));
        return res;
      } else {
        isSubstitution = true;
      }
      auto res = move(entry);
      return res;
    }
  }
  case 'I': {
    return demangleTemplate(++ptr, substitutions, templates);
  }
  case 'T': {
    auto *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'V': {
      ++ptr;
      bool useless;
      auto type = demangleType(ptr, substitutions, templates, useless);
      if (type.failed())
        return type;
      return new VirtualSymbol(VirtualSymbol::VTABLE, move(*type));
    }
    case 'T': {
      ++ptr;
      bool useless;
      auto type = demangleType(ptr, substitutions, templates, useless);
      if (type.failed())
        return type;
      return new VirtualSymbol(VirtualSymbol::VTT, move(*type));
    }
    case 'I': {
      ++ptr;
      bool useless;
      auto type = demangleType(ptr, substitutions, templates, useless);
      if (type.failed())
        return type;
      return new VirtualSymbol(VirtualSymbol::TYPEINFO, move(*type));
    }
    case 'S': {
      ++ptr;
      bool useless;
      auto type = demangleType(ptr, substitutions, templates, useless);
      if (type.failed())
        return type;
      return new VirtualSymbol(VirtualSymbol::TYPEINFO_NAME, move(*type));
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
  }
  case 'C': {
    auto *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case '1':
    case '2':
    case '3': {
      ++ptr;
      return new ConstructorSymbol;
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
  }
  case 'D': {
    auto *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case '0':
    case '1':
    case '2': {
      ++ptr;
      return new DestructorSymbol;
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
  }
  case 'Z': {
    ++ptr;
    auto inner = demangle<true>(ptr, substitutions, templates);
    if (inner.failed())
      return inner;
    if (*ptr != 'E') {
      return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
          "Unterminated scope (Z...E). [" + String(ptr) + "].");
    }
    ++ptr;
    bool useless;
    auto name = demangleName(ptr, substitutions, templates, useless);
    if (name.failed())
      return name;
    auto *result = new ChainSymbol;
    result->addPart(move(*inner));
    result->addPart(move(*name));
    return result;
  }
  case 'N': {
    ++ptr;
    return demangleNestedName(ptr, substitutions, templates);
  }
  case 'U': {
    auto *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'l': {
      ++ptr;
      auto *lambda = new LambdaSymbol;
      while (*ptr != 'E') {
        bool useless;
        auto t = demangleType(ptr, substitutions, templates, useless);
        if (t.failed()) {
          delete lambda;
          return t;
        }
        lambda->addType(move(*t));
      }
      ++ptr;
      u64 index;
      if (*ptr == '_') {
        index = 1;
      } else {
        parseSize(index, ptr);
        index += 2;
      }
      if (*ptr != '_') {
        delete lambda;
        return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
            String("Expected '_' at the end of a lambda. [") + ptr + "].");
      }
      ++ptr;
      return lambda;
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
  }
  case 'G': {
    auto *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'V': {
      ++ptr;
      bool useless;
      auto inner = demangleName(ptr, substitutions, templates, useless);
      if (inner.failed())
        return inner;
      return new VirtualSymbol(VirtualSymbol::GUARD, move(*inner));
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
  }
  default: {
  defaultDemangle:
    u64 size;
    parseSize(size, ptr);
    if (size == 0) {
      return LogicalResult<void>::failure(
          String("Parsed a symbol name size of 0. [") + ptr + "]");
    }
    String name = "";
    for (u64 i = 0; i < size; ++i, ++ptr) {
      if (*ptr == 0) {
        return LogicalResult<void>::failure(
            "Name terminated during the parsing of a value.");
      }
      name += *ptr;
    }
    auto *res = new BaseSymbol(name);
    // substitutions.pushBack(res->clone());
    return res;
  }
  }
}

LogicalResult<UniquePtr<DemangledSymbol>>
demangleModule(const char *(&ptr),
               Vector<UniquePtr<DemangledSymbol>> &substitutions,
               Vector<UniquePtr<DemangledSymbol>> &templates) {
  u64 size;
  parseSize(size, ptr);
  if (size == 0) {
    return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
        "Zero-sized module name.");
  }
  String moduleName = "";
  for (u64 i = 0; i < size; ++i, ++ptr) {
    if (*ptr == 0) {
      return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
          "End of name during module demangling.");
    }
    moduleName += *ptr;
  }
  auto *moduleBase = new ModuleBaseSymbol(moduleName);
  substitutions.pushBack(moduleBase->clone());
  bool useless;
  auto base = demangleName(ptr, substitutions, templates, useless);
  if (base.failed()) {
    return base;
  }
  return new ModuleSymbol(move(*base), moduleBase);
}

LogicalResult<UniquePtr<DemangledSymbol>> demangleType(
    const char *(&ptr), Vector<UniquePtr<DemangledSymbol>> &substitutions,
    Vector<UniquePtr<DemangledSymbol>> &templates, bool &isSubstitution,
    Vector<UniquePtr<DemangledSymbol>> *currentTemplates) {
  isSubstitution = false;
  switch (*ptr) {
  case 'V': {
    ++ptr;
    bool useless;
    auto inner = demangleType(ptr, substitutions, templates, useless);
    if (inner.failed())
      return inner;
    return new VolatileSymbol(move(*inner));
  }
  case 'v': {
    ++ptr;
    return new BuiltinSymbol("void");
  }
  case 'w': {
    ++ptr;
    return new BuiltinSymbol("wchar_t");
  }
  case 'b': {
    ++ptr;
    return new BuiltinSymbol("bool");
  }
  case 'c': {
    ++ptr;
    return new BuiltinSymbol("char");
  }
  case 'a': {
    ++ptr;
    return new BuiltinSymbol("signed char");
  }
  case 'h': {
    ++ptr;
    return new BuiltinSymbol("unsigned char");
  }
  case 's': {
    ++ptr;
    return new BuiltinSymbol("short");
  }
  case 't': {
    ++ptr;
    return new BuiltinSymbol("unsigned short");
  }
  case 'i': {
    ++ptr;
    return new BuiltinSymbol("int");
  }
  case 'I': {
    return demangleTemplate(++ptr, substitutions, templates);
  }
  case 'j': {
    ++ptr;
    return new BuiltinSymbol("unsigned int");
  }
  case 'l': {
    ++ptr;
    return new BuiltinSymbol("long");
  }
  case 'm': {
    ++ptr;
    return new BuiltinSymbol("unsigned long");
  }
  case 'x': {
    ++ptr;
    return new BuiltinSymbol("long long");
  }
  case 'y': {
    ++ptr;
    return new BuiltinSymbol("unsigned long long");
  }
  case 'n': {
    ++ptr;
    return new BuiltinSymbol("__int128");
  }
  case 'o': {
    ++ptr;
    return new BuiltinSymbol("unsigned __int128");
  }
  case 'f': {
    ++ptr;
    return new BuiltinSymbol("float");
  }
  case 'd': {
    ++ptr;
    return new BuiltinSymbol("double");
  }
  case 'e': {
    ++ptr;
    return new BuiltinSymbol("long double");
  }
  case 'g': {
    ++ptr;
    return new BuiltinSymbol("__float128");
  }
  case 'z': {
    ++ptr;
    return new BuiltinSymbol("...");
  }
  case 'P': {
    bool isSubstitution;
    auto innerType =
        demangleType(++ptr, substitutions, templates, isSubstitution);
    if (innerType.failed())
      return innerType;
    if (!isSubstitution && (*innerType)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*innerType)->clone());
    auto &t = *innerType;
    if (t->t == DemangledSymbol::PTR) {
      auto *ptr = static_cast<PtrSymbol *>(t.getRaw());
      ++ptr->depth;
      return move(t);
    }
    auto *res = new PtrSymbol(move(*innerType));
    // substitutions.pushBack(res->clone());
    return res;
  }
  case 'R': {
    bool isSubstitution;
    auto innerType =
        demangleType(++ptr, substitutions, templates, isSubstitution);
    if (innerType.failed())
      return innerType;
    if (!isSubstitution && (*innerType)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*innerType)->clone());
    auto *res = new LvalSymbol(move(*innerType));
    // substitutions.pushBack(res->clone());
    return res;
  }
  case 'O': {
    bool isSubstitution;
    auto innerType =
        demangleType(++ptr, substitutions, templates, isSubstitution);
    if (innerType.failed())
      return innerType;
    if (!isSubstitution && (*innerType)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*innerType)->clone());
    auto *res = new RvalSymbol(move(*innerType));
    // substitutions.pushBack(res->clone());
    return res;
  }
  case 'K': {
    bool isSubstitution;
    auto innerType =
        demangleType(++ptr, substitutions, templates, isSubstitution);
    if (innerType.failed())
      return innerType;
    if (!isSubstitution && (*innerType)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*innerType)->clone());
    auto *res = new ConstSymbol(move(*innerType));
    return res;
  }
  case 'F': {
    bool isSubstitution;
    auto returnType =
        demangleType(++ptr, substitutions, templates, isSubstitution);
    if (returnType.failed())
      return returnType;
    if (!isSubstitution && (*returnType)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*returnType)->clone());
    auto *resultType = new FunTypeSymbol(move(*returnType));
    while (*ptr != 'E') {
      auto argType =
          demangleType(ptr, substitutions, templates, isSubstitution);
      if (argType.failed())
        return argType;
      if (!isSubstitution && (*argType)->t != DemangledSymbol::BUILTIN)
        substitutions.pushBack((*argType)->clone());
      resultType->addArgument(move(*argType));
    }
    ++ptr;
    // substitutions.pushBack(resultType->clone());
    return resultType;
  }
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    bool useless;
    auto nameResult = demangleName(ptr, substitutions, templates, useless);
    if (nameResult.failed())
      return nameResult;
    auto res = move(*nameResult);
    return res;
  }
  case 'N': {
    ++ptr;
    auto *chain = new ChainSymbol();
    bool wasSubstitution = true;
    while (*ptr != 'E') {
      if (!wasSubstitution)
        substitutions.pushBack(chain->clone());
      wasSubstitution = false;
      auto inner = demangleType(ptr, substitutions, templates, wasSubstitution);
      if (inner.failed()) {
        return inner;
      }
      chain->addPart(move(*inner));
    }
    ++ptr;
    return chain;
  }
  case 'S': {
    ++ptr;
    if (*ptr == 't') {
      ++ptr;
      return new StdSymbol;
    } else {
      u64 index;
      parseSubstitutionIndex(index, ptr);
      assert(substitutions.length() > index);
      auto entry = substitutions[index]->clone();
      if (entry->t == DemangledSymbol::MODULEBASE) {
        bool useless;
        auto next = demangleType(ptr, substitutions, templates, useless);
        if (next.failed()) {
          return next;
        }
        return new ModuleSymbol(move(*next), move(entry));
      } else {
        isSubstitution = true;
      }
      return entry;
    }
  }
  case 'T': {
    ++ptr;
    switch (*ptr) {
    case 'L': {
      ++ptr;
      u64 size;
      parseSubstitutionIndex(size, ptr);
      assert(currentTemplates != nullptr);
      assert(currentTemplates->length() >= size);
      assert(*ptr == '_');
      ++ptr;
      return (*currentTemplates)[size - 1]->clone();
    }
    default: {
      u64 size;
      parseSubstitutionIndex(size, ptr);
      assert(templates.length() > size);
      return templates[size]->clone();
    }
    }
  }
  case 'W': {
    ++ptr;
    return demangleModule(ptr, substitutions, templates);
  }
  case 'D': {
    auto *savePtr = ptr;
    ++ptr;
    switch (*ptr) {
    case 'p': {
      ++ptr;
      bool useless;
      auto t = demangleType(ptr, substitutions, templates, useless);
      if ((*t)->t != DemangledSymbol::TEMPLATE) {
        return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
            "Pack expension of something that is not a template pack.");
      }
      static_cast<TemplateSymbol *>(t->getRaw())->printLimits = false;
      return t;
    }
    case 'n': {
      ++ptr;
      return new BuiltinSymbol("std::nullptr_t");
    }
    default: {
      ptr = savePtr;
      goto defaultDemangle;
    }
    }
  }
  default: {
  defaultDemangle:
    return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
        String("Type starting with '") + *ptr + "' is not yet supported. [" +
        ptr + "]");
  }
  }
}

Pair<String, String> generateTypePrefixSuffix(UniquePtr<DemangledSymbol> type) {
  Map<String, String> prefixes = {{"void", "(void)"},
                                  {"wchar_t", "(wchar_t)"},
                                  {"char", "(char)"},
                                  {"signed char", "(signed char)"},
                                  {"unsigned char", "(unsigned char)"},
                                  {"short", "(short)"},
                                  {"unsigned short", "(unsigned short)"},
                                  {"__int128", "(__int128)"},
                                  {"unsigned __int128", "(unsigned __int128)"},
                                  {"...", "(...)"}

  };
  Map<String, String> suffixes = {{"unsigned int", "u"},
                                  {"long", "l"},
                                  {"unsigned long", "ul"},
                                  {"long long", "ll"},
                                  {"unsigned long long", "ull"}};
  switch (type->t) {
  case DemangledSymbol::BASE: {
    auto *t = static_cast<BaseSymbol *>(type.getRaw());
    auto prefix = prefixes.contains(t->name) ? prefixes.get(t->name) : "";
    auto suffix = suffixes.contains(t->name) ? suffixes.get(t->name) : "";
    return Pair<String, String>(prefix, suffix);
  }
  default:
    return Pair<String, String>("", "");
  }
}

LogicalResult<UniquePtr<DemangledSymbol>>
demangleExprPrimary(const char *(&ptr),
                    Vector<UniquePtr<DemangledSymbol>> &substitutions,
                    Vector<UniquePtr<DemangledSymbol>> &templates) {
  bool useless;
  auto t = demangleType(ptr, substitutions, templates, useless);
  if (t.failed())
    return t;
  bool neg = false;
  if (*ptr == 'n') {
    neg = true;
    ++ptr;
  }
  u64 value = 0;
  while ((*ptr >= '0') && (*ptr <= '9')) {
    value = 10 * value + (*ptr - '0');
    ++ptr;
  }
  assert(*ptr == 'E');
  ++ptr;
  auto [prefix, suffix] = generateTypePrefixSuffix(move(*t));
  String resultName = prefix + (neg ? "-" : "") + String::of(value) + suffix;
  return new BaseSymbol(resultName);
}

template <bool isInRequire = false>
LogicalResult<UniquePtr<DemangledSymbol>>
demangleExpression(const char *(&ptr),
                   Vector<UniquePtr<DemangledSymbol>> &substitutions,
                   Vector<UniquePtr<DemangledSymbol>> &templates) {
  // TODO: expression
  switch (*ptr) {
  case 'L': {
    ++ptr;
    return demangleExprPrimary(ptr, substitutions, templates);
  }
  default: {
    u64 size;
    parseSize(size, ptr);
    if (size == 0) {
      return LogicalResult<UniquePtr<DemangledSymbol>>::failure(
          "Simple-id of size 0.\n");
    }
    String name = "";
    for (u64 i = 0; i < size; ++i, ++ptr) {
      name += *ptr;
    }
    auto *n = new BaseSymbol(name);
    if (*ptr == 'I') {
      ++ptr;
      auto t = demangleTemplate(ptr, substitutions, templates);
      if (t.failed()) {
        delete n;
        return t;
      }
      auto *symbol = new Symbol(n);
      symbol->hasTemplate_ = true;
      symbol->templates = move(*t);
      return symbol;
    }
    return n;
  }
  }
}

LogicalResult<UniquePtr<DemangledSymbol>>
demangleTemplate(const char *(&ptr),
                 Vector<UniquePtr<DemangledSymbol>> &substitutions,
                 Vector<UniquePtr<DemangledSymbol>> &templates) {
  auto *res = new TemplateSymbol;
  while (*ptr != 'E') {
    switch (*ptr) {
    case 'L': {
      auto expr = demangleExprPrimary(++ptr, substitutions, templates);
      if (expr.failed()) {
        delete res;
        return expr;
      }
      templates.pushBack((*expr)->clone());
      res->addTemplate(move(*expr));
      break;
    }
    case 'J': {
      ++ptr;
      Vector<UniquePtr<DemangledSymbol>> innerTemplates;
      auto pack = demangleTemplate(ptr, substitutions, innerTemplates);
      if (pack.failed()) {
        delete res;
        return pack;
      }
      auto *packTemplates = static_cast<TemplateSymbol *>(pack->getRaw());
      for (auto &t : packTemplates->templates) {
        res->addTemplate(t->clone());
      }
      templates.pushBack(move(*pack));
      break;
    }
    case 'Q': {
      ++ptr;
      auto *rqs = new RequireSymbol;
      auto expr = demangleExpression<true>(ptr, substitutions, templates);
      if (expr.failed()) {
        delete res;
        delete rqs;
        return expr;
      }
      rqs->addCond(move(*expr));
      res->rqs = rqs;
      break;
    }
    default: {
      bool isSubstitution;
      auto t = demangleType(ptr, substitutions, templates, isSubstitution,
                            &res->templates);
      if (t.failed()) {
        delete res;
        return t;
      }
      if (!isSubstitution && (*t)->t != DemangledSymbol::BUILTIN)
        substitutions.pushBack((*t)->clone());
      templates.pushBack((*t)->clone());
      res->addTemplate(move(*t));
      break;
    }
    }
  }
  // substitutions.pushBack(res->clone());
  ++ptr;
  return res;
}

template <bool END_ON_E = false>
LogicalResult<UniquePtr<DemangledSymbol>>
demangle(const char *(&ptr), Vector<UniquePtr<DemangledSymbol>> &substitutions,
         Vector<UniquePtr<DemangledSymbol>> &templates) {
  UniquePtr<DemangledSymbol> name;
  bool isConst = false;
  if (*ptr == 'N') {
    auto demangleNestedNameResult =
        demangleNestedName(++ptr, substitutions, templates);
    if (demangleNestedNameResult.failed())
      return demangleNestedNameResult;
    name = move(*demangleNestedNameResult);
  } else {
    if (*ptr == 'K') {
      isConst = true;
      ++ptr;
    }
    if (*ptr == 'W') {
      auto mod = demangleModule(++ptr, substitutions, templates);
      if (mod.failed())
        return mod;
      substitutions.pushBack((*mod)->clone());
      name = move(*mod);
    } else {
      bool isSubstitution = false;
      auto demangledName =
          demangleName(ptr, substitutions, templates, isSubstitution);
      if (demangledName.failed())
        return demangledName;
      if (!isSubstitution)
        substitutions.pushBack((*demangledName)->clone());
      name = move(*demangledName);
    }
  }
  auto result = new Symbol(move(name));
  result->isConst = isConst;
  if (*ptr == 0)
    return result;
  if (*ptr == 'I') {
    auto tmplts = demangleTemplate(++ptr, substitutions, templates);
    if (tmplts.failed()) {
      return tmplts;
    }
    result->hasTemplate_ = true;
    result->templates = move(*tmplts);
  }
  if (*ptr == 0)
    return result;
  if (result->hasTemplate() && !result->isConstructor()) {
    bool isSubstitution;
    auto retType = demangleType(ptr, substitutions, templates, isSubstitution);
    if (retType.failed()) {
      return retType;
    }
    if (!isSubstitution && (*retType)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*retType)->clone());
    result->hasReturnType = true;
    result->returnType = move(*retType);
  }
  if (*ptr == 0)
    return result;
  if constexpr (END_ON_E) {
    if (*ptr == 'E')
      return result;
  }
  result->hasType = true;
  while (*ptr != 0) {
    if constexpr (END_ON_E) {
      if (*ptr == 'E')
        return result;
    }
    bool isSubstitution;
    auto type = demangleType(ptr, substitutions, templates, isSubstitution);
    if (type.failed()) {
      return type;
    }
    if (!isSubstitution && (*type)->t != DemangledSymbol::BUILTIN)
      substitutions.pushBack((*type)->clone());
    result->types.pushBack(move(*type));
  }
  return result;
}

// https://itanium-cxx-abi.github.io/cxx-abi/abi.html#mangling
export LogicalResult<String> demangle(String name) {
  // Not mangled
  if (!name.startsWith("_Z"))
    return name;

  Vector<UniquePtr<DemangledSymbol>> substitutions, templates;
  auto *ptr = name.ptr() + 2;
  auto demangleResult = demangle(ptr, substitutions, templates);
  if (demangleResult.failed())
    return demangleResult;

  String result = (*demangleResult)->toString();

  return result;
}
} // namespace core