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
export module ast:tokens;

import core;

namespace ast::tokens {

export enum TokenType {
  DOUBLE_MINUS,
  MINUS_EQUAL,
  ARROW,
  MINUS,
  COMMA,
  SEMI,
  COLON,
  EXCLAMATION_EQUAL,
  EXCLAMATION,
  QUESTION,
  DOT,
  LP,
  RP,
  LSB,
  RSB,
  LB,
  RB,
  STAR_EQUAL,
  STAR,
  SLASH_EQUAL,
  SLASH,
  DOUBLE_AMPERSAND_EQUAL,
  DOUBLE_AMPERSAND,
  AMPERSAND_EQUAL,
  AMPERSAND,
  PERCENT_EQUAL,
  PERCENT,
  DOUBLE_WEDGE_EQUAL,
  DOUBLE_WEDGE,
  WEDGE_EQUAL,
  WEDGE,
  DOUBLE_PLUS,
  PLUS_EQUAL,
  PLUS,
  DOUBLE_LAB_EQUAL,
  DOUBLE_LAB,
  LAB_EQUAL,
  LAB,
  DOUBLE_EQUAL,
  EQUAL,
  DOUBLE_RAB_EQUAL,
  DOUBLE_RAB,
  RAB_EQUAL,
  RAB,
  DOUBLE_PIPE_EQUAL,
  DOUBLE_PIPE,
  PIPE_EQUAL,
  PIPE,
  TILDE,
  ALIAS,
  BIT,
  BITCAST,
  BREAK,
  CASE,
  CAST,
  CONTINUE,
  DEFAULT,
  DO,
  ELSE,
  FALSE,
  FOR,
  FUN,
  IF,
  OPERATOR,
  OTHER,
  RETURN,
  SIZEOF,
  STRUCT,
  SWITCH,
  SYSCALL,
  THIS,
  TRUE,
  UNION,
  VAR,
  VOID,
  WHILE,

  INT,
  CHAR,
  STRING,
  ID,

  UNKNOWN
};

namespace {
bool noArgToken(TokenType type) {
  switch (type) {
  case STRING:
  case ID:
  case INT:
  case CHAR:
    return false;
  default:
    return true;
  }
}

bool stringArgToken(TokenType type) { return (type == STRING) || (type == ID); }

bool intArgToken(TokenType type) { return type == INT; }

bool charArgToken(TokenType type) { return type == CHAR; }

} // namespace

export struct Token {
  TokenType type;
  union {
    u64 i;
    char c;
    core::String s;
  };
  u64 start_line, start_column, end_line, end_column;

  ~Token() {
    switch (type) {
    case STRING:
    case ID:
      s.~String();
    default:;
    }
  }

  Token()
      : type(UNKNOWN), i(0), start_line(0), start_column(0), end_line(0),
        end_column(0) {}

  Token(const Token &other)
      : type(other.type), start_line(other.start_line),
        start_column(other.start_column), end_line(other.end_line),
        end_column(other.end_column) {
    switch (type) {
    case STRING:
    case ID:
      s = other.s;
      break;
    case INT:
      i = other.i;
      break;
    case CHAR:
      c = other.c;
      break;
    default:
      i = 0;
    }
  }

  Token(Token &&other)
      : type(core::move(other.type)), start_line(other.start_line),
        start_column(other.start_column), end_line(other.end_line),
        end_column(other.end_column) {
    switch (type) {
    case STRING:
    case ID:
      s = core::move(other.s);
      break;
    case INT:
      i = core::move(other.i);
      break;
    case CHAR:
      c = core::move(other.c);
      break;
    default:
      i = 0;
    }
  }

  Token &operator=(const Token &other) {
    type = other.type;
    start_line = other.start_line;
    start_column = other.start_column;
    end_line = other.end_line;
    end_column = other.end_column;
    switch (type) {
    case STRING:
    case ID:
      s.~String();
      s = other.s;
      break;
    case INT:
      i = other.i;
      break;
    case CHAR:
      c = other.c;
      break;
    default:
      i = 0;
    }
    return *this;
  }

  Token &operator=(Token &&other) {
    type = core::move(other.type);
    start_line = other.start_line;
    start_column = other.start_column;
    end_line = other.end_line;
    end_column = other.end_column;
    switch (type) {
    case STRING:
    case ID:
      s.~String();
      s = core::move(other.s);
      break;
    case INT:
      i = core::move(other.i);
      break;
    case CHAR:
      c = core::move(other.c);
      break;
    default:
      i = 0;
    }
    return *this;
  }

  Token(TokenType type, u64 start_line, u64 start_column, u64 end_line,
        u64 end_column)
      : type(type), i(0), start_line(start_line), start_column(start_column),
        end_line(end_line), end_column(end_column) {
    core::assert(noArgToken(type));
  }

  Token(TokenType type, u64 i, u64 start_line, u64 start_column, u64 end_line,
        u64 end_column)
      : type(type), i(i), start_line(start_line), start_column(start_column),
        end_line(end_line), end_column(end_column) {
    core::assert(intArgToken(type));
  }

  Token(TokenType type, char c, u64 start_line, u64 start_column, u64 end_line,
        u64 end_column)
      : type(type), c(c), start_line(start_line), start_column(start_column),
        end_line(end_line), end_column(end_column) {
    core::assert(charArgToken(type));
  }

  Token(TokenType type, core::String s, u64 start_line, u64 start_column,
        u64 end_line, u64 end_column)
      : type(type), s(s), start_line(start_line), start_column(start_column),
        end_line(end_line), end_column(end_column) {
    core::assert(stringArgToken(type));
  }
};

export core::String to_string(TokenType type) {
  switch (type) {
  case DOUBLE_MINUS:
    return "doubleMinus";
  case MINUS_EQUAL:
    return "minusEqual";
  case ARROW:
    return "arrow";
  case MINUS:
    return "minus";
  case COMMA:
    return "comma";
  case SEMI:
    return "semi";
  case COLON:
    return "colon";
  case EXCLAMATION_EQUAL:
    return "exclamationEqual";
  case EXCLAMATION:
    return "exclamation";
  case QUESTION:
    return "question";
  case DOT:
    return "dot";
  case LP:
    return "lp";
  case RP:
    return "rp";
  case LSB:
    return "lsb";
  case RSB:
    return "rsb";
  case LB:
    return "lb";
  case RB:
    return "rb";
  case STAR_EQUAL:
    return "starEqual";
  case STAR:
    return "star";
  case SLASH_EQUAL:
    return "slashEqual";
  case SLASH:
    return "slash";
  case DOUBLE_AMPERSAND_EQUAL:
    return "doubleAmpersandEqual";
  case DOUBLE_AMPERSAND:
    return "doubleAmpersand";
  case AMPERSAND_EQUAL:
    return "ampersandEqual";
  case AMPERSAND:
    return "ampersand";
  case PERCENT_EQUAL:
    return "percentEqual";
  case PERCENT:
    return "percent";
  case DOUBLE_WEDGE_EQUAL:
    return "doubleWedgeEqual";
  case DOUBLE_WEDGE:
    return "doubleWedge";
  case WEDGE_EQUAL:
    return "wedgeEqual";
  case WEDGE:
    return "wedge";
  case DOUBLE_PLUS:
    return "doublePlus";
  case PLUS_EQUAL:
    return "plusEqual";
  case PLUS:
    return "plus";
  case DOUBLE_LAB_EQUAL:
    return "doubleLabEqual";
  case DOUBLE_LAB:
    return "doubleLab";
  case LAB_EQUAL:
    return "labEqual";
  case LAB:
    return "lab";
  case DOUBLE_EQUAL:
    return "doubleEqual";
  case EQUAL:
    return "equal";
  case DOUBLE_RAB_EQUAL:
    return "doubleRabEqual";
  case DOUBLE_RAB:
    return "doubleRab";
  case RAB_EQUAL:
    return "rabEqual";
  case RAB:
    return "rab";
  case DOUBLE_PIPE_EQUAL:
    return "doublePipeEqual";
  case DOUBLE_PIPE:
    return "doublePipe";
  case PIPE_EQUAL:
    return "pipeEqual";
  case PIPE:
    return "pipe";
  case TILDE:
    return "tidle";
  case ALIAS:
    return "alias";
  case BIT:
    return "bit";
  case BITCAST:
    return "bitcast";
  case BREAK:
    return "break";
  case CASE:
    return "case";
  case CAST:
    return "cast";
  case CONTINUE:
    return "continue";
  case DEFAULT:
    return "default";
  case DO:
    return "do";
  case ELSE:
    return "else";
  case FALSE:
    return "false";
  case FOR:
    return "for";
  case FUN:
    return "fun";
  case IF:
    return "if";
  case OPERATOR:
    return "operator";
  case OTHER:
    return "other";
  case RETURN:
    return "return";
  case SIZEOF:
    return "sizeof";
  case STRUCT:
    return "struct";
  case SWITCH:
    return "switch";
  case SYSCALL:
    return "syscall";
  case THIS:
    return "this";
  case TRUE:
    return "true";
  case UNION:
    return "union";
  case VAR:
    return "var";
  case VOID:
    return "void";
  case WHILE:
    return "while";
  case INT:
    return "int";
  case CHAR:
    return "char";
  case STRING:
    return "string";
  case ID:
    return "id";
  case UNKNOWN:
    return "unknown";
    break;
  }
}

export const core::String to_string(Token tkn) {
  core::String result = to_string(tkn.type) + "(";
  switch (tkn.type) {
  case INT: {
    result += core::String::of(tkn.i) + ", ";
    break;
  }
  case CHAR: {
    result += "'" + core::String::of(tkn.c) + "', ";
    break;
  }
  case STRING: {
    result += "\"" + tkn.s + "\", ";
    break;
  }
  case ID: {
    result += tkn.s + ", ";
    break;
  }
  default: {
    break;
  }
  }
  return result + core::String::of(tkn.start_line) + ", " +
         core::String::of(tkn.start_column) + ", " +
         core::String::of(tkn.end_line) + ", " +
         core::String::of(tkn.end_column) + ")";
}

} // namespace ast::tokens
