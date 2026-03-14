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
export module frontend:lexer;

import core;
import ast;

namespace compiler::frontend {

using enum ast::tokens::TokenType;

namespace {

core::LogicalResult<core::Vector<ast::tokens::Token>>
lexingError(const core::String message, u64 line, u64 column) {
  return core::LogicalResult<core::Vector<ast::tokens::Token>>::failure(
      "Lexing error at " + core::String::of(line) + ":" +
      core::String::of(column) + ":\n" + message);
}

core::LogicalResult<char> escape(char c) {
  switch (c) {
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 't':
    return '\t';
  case 'a':
    return '\a';
  case 'f':
    return '\f';
  case '\\':
    return '\\';
  case '"':
    return '\"';
  case '\'':
    return '\'';
  default:
    return core::LogicalResult<char>::failure("Unknown escaped sequence \\" +
                                              core::String::of(c) + ".");
  }
}

} // namespace

core::LogicalResult<core::Vector<ast::tokens::Token>>
lex(core::String::Iterator it, core::String::Iterator end) {
  core::Vector<ast::tokens::Token> result;
  u64 line = 1, column = 1;
  while (it != end) {
    u64 line_s = line, column_s = column;
    switch (*it) {
    case '-': {
      ++column;
      ++it;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){MINUS, line_s, column_s, line, column});
        break;
      }
      switch (*it) {
      case '-': {
        ++column;
        ++it;
        result.pushBack(
            (ast::tokens::Token){DOUBLE_MINUS, line_s, column_s, line, column});
        break;
      }

      case '=': {
        ++column;
        ++it;
        result.pushBack(
            (ast::tokens::Token){MINUS_EQUAL, line_s, column_s, line, column});
        break;
      }

      case '>': {
        ++column;
        ++it;
        result.pushBack(
            (ast::tokens::Token){ARROW, line_s, column_s, line, column});
        break;
      }

      default: {
        result.pushBack(
            (ast::tokens::Token){MINUS, line_s, column_s, line, column});
        break;
      }
      }
      break;
    }

    case ',': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){COMMA, line_s, column_s, line, column});
      break;
    }

    case ';': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){SEMI, line_s, column_s, line, column});
      break;
    }

    case ':': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){COLON, line_s, column_s, line, column});
      break;
    }

    case '!': {
      ++it;
      ++column;
      if ((it != end) && (*it == '=')) {
        ++it;
        ++column;
        result.pushBack((ast::tokens::Token){EXCLAMATION_EQUAL, line_s,
                                             column_s, line, column});
      } else {
        result.pushBack(
            (ast::tokens::Token){EXCLAMATION, line_s, column_s, line, column});
      }
      break;
    }

    case '?': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){QUESTION, line_s, column_s, line, column});
      break;
    }

    case '.': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){DOT, line_s, column_s, line, column});
      break;
    }

    case '(': {
      ++it;
      ++column;
      result.pushBack((ast::tokens::Token){LP, line_s, column_s, line, column});
      break;
    }

    case ')': {
      ++it;
      ++column;
      result.pushBack((ast::tokens::Token){RP, line_s, column_s, line, column});
      break;
    }

    case '[': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){LSB, line_s, column_s, line, column});
      break;
    }

    case ']': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){RSB, line_s, column_s, line, column});
      break;
    }

    case '{': {
      ++it;
      ++column;
      result.pushBack((ast::tokens::Token){LB, line_s, column_s, line, column});
      break;
    }

    case '}': {
      ++it;
      ++column;
      result.pushBack((ast::tokens::Token){RB, line_s, column_s, line, column});
      break;
    }

    case '*': {
      ++it;
      ++column;
      if ((it != end) && (*it == '=')) {
        ++it;
        ++column;
        result.pushBack(
            (ast::tokens::Token){STAR_EQUAL, line_s, column_s, line, column});
      } else {
        result.pushBack(
            (ast::tokens::Token){STAR, line_s, column_s, line, column});
      }
      break;
    }

    case '/': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){SLASH, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '=': {
          ++it;
          ++column;
          result.pushBack((ast::tokens::Token){SLASH_EQUAL, line_s, column_s,
                                               line, column});
        }

        case '/': {
          ++it;
          ++column;
          bool finished = it == end;
          while (!finished) {
            if ((*it) == '\n') {
              ++it;
              ++line;
              column = 1;
              finished = true;
            } else {
              ++it;
              ++column;
            }
            if (it == end)
              finished = true;
          }
          break;
        }

        case '*': {
          char last = '/';
          bool finished = false;
          while (!finished) {
            if (it == end)
              return lexingError("End-of-file in multi-line comment.", line,
                                 column);
            switch (*it) {
            case '/': {
              ++it;
              ++column;
              if (last == '*')
                finished = true;
              last = '/';
              break;
            }

            case '\n': {
              ++it;
              ++line;
              column = 1;
              last = '\n';
              break;
            }

            case '\\': {
              ++it;
              ++column;
              if (it == end) {
                return lexingError(
                    "End-of-file in escaped characted in multi-line comment.",
                    line, column);
              }
              ++it;
              ++column;
              last = 0; // don't care
              break;
            }

            default: {
              last = *it;
              ++it;
              ++column;
              break;
            }
            }
          }
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){SLASH, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '&': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){AMPERSAND, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '&': {
          ++it;
          ++column;
          if ((it != end) && (*it == '=')) {
            ++it;
            ++column;
            result.pushBack((ast::tokens::Token){DOUBLE_AMPERSAND_EQUAL, line_s,
                                                 column_s, line, column});
          } else {
            result.pushBack((ast::tokens::Token){DOUBLE_AMPERSAND, line_s,
                                                 column_s, line, column});
          }
          break;
        }

        case '=': {
          ++it;
          ++column;
          result.pushBack((ast::tokens::Token){AMPERSAND_EQUAL, line_s,
                                               column_s, line, column});
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){AMPERSAND, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '%': {
      ++it;
      ++column;
      if ((it != end) && (*it == '=')) {
        ++it;
        ++column;
        result.pushBack((ast::tokens::Token){PERCENT_EQUAL, line_s, column_s,
                                             line, column});
      } else {
        result.pushBack(
            (ast::tokens::Token){PERCENT, line_s, column_s, line, column});
      }
      break;
    }

    case '^': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){WEDGE, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '^': {
          ++it;
          ++column;
          if ((it != end) && (*it == '=')) {
            ++it;
            ++column;
            result.pushBack((ast::tokens::Token){DOUBLE_WEDGE_EQUAL, line_s,
                                                 column_s, line, column});
          } else {
            result.pushBack((ast::tokens::Token){DOUBLE_WEDGE, line_s, column_s,
                                                 line, column});
          }
          break;
        }

        case '=': {
          ++it;
          ++column;
          result.pushBack((ast::tokens::Token){WEDGE_EQUAL, line_s, column_s,
                                               line, column});
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){WEDGE, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '+': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){PLUS, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '+': {
          ++it;
          ++column;
          result.pushBack((ast::tokens::Token){DOUBLE_PLUS, line_s, column_s,
                                               line, column});
          break;
        }

        case '=': {
          ++it;
          ++column;
          result.pushBack(
              (ast::tokens::Token){PLUS_EQUAL, line_s, column_s, line, column});
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){PLUS, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '<': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){LAB, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '<': {
          ++it;
          ++column;
          if ((it != end) && (*it == '=')) {
            ++it;
            ++column;
            result.pushBack((ast::tokens::Token){DOUBLE_LAB_EQUAL, line_s,
                                                 column_s, line, column});
          } else {
            result.pushBack((ast::tokens::Token){DOUBLE_LAB, line_s, column_s,
                                                 line, column});
          }
          break;
        }

        case '=': {
          ++it;
          ++column;
          result.pushBack(
              (ast::tokens::Token){LAB_EQUAL, line_s, column_s, line, column});
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){LAB, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '=': {
      ++it;
      ++column;
      if ((it != end) && (*it == '=')) {
        ++it;
        ++column;
        result.pushBack(
            (ast::tokens::Token){DOUBLE_EQUAL, line_s, column_s, line, column});
      } else {
        result.pushBack(
            (ast::tokens::Token){EQUAL, line_s, column_s, line, column});
      }
      break;
    }

    case '>': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){RAB, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '>': {
          ++it;
          ++column;
          if ((it != end) && (*it == '=')) {
            ++it;
            ++column;
            result.pushBack((ast::tokens::Token){DOUBLE_RAB_EQUAL, line_s,
                                                 column_s, line, column});
          } else {
            result.pushBack((ast::tokens::Token){DOUBLE_RAB, line_s, column_s,
                                                 line, column});
          }
          break;
        }

        case '=': {
          ++it;
          ++column;
          result.pushBack(
              (ast::tokens::Token){RAB_EQUAL, line_s, column_s, line, column});
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){RAB, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '|': {
      ++it;
      ++column;
      if (it == end) {
        result.pushBack(
            (ast::tokens::Token){PIPE, line_s, column_s, line, column});
      } else {
        switch (*it) {
        case '|': {
          ++it;
          ++column;
          if ((it != end) && (*it == '=')) {
            ++it;
            ++column;
            result.pushBack((ast::tokens::Token){DOUBLE_PIPE_EQUAL, line_s,
                                                 column_s, line, column});
          } else {
            result.pushBack((ast::tokens::Token){DOUBLE_PIPE, line_s, column_s,
                                                 line, column});
          }
          break;
        }

        case '=': {
          ++it;
          ++column;
          result.pushBack(
              (ast::tokens::Token){PIPE_EQUAL, line_s, column_s, line, column});
          break;
        }

        default: {
          result.pushBack(
              (ast::tokens::Token){PIPE, line_s, column_s, line, column});
          break;
        }
        }
      }
      break;
    }

    case '~': {
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){TILDE, line_s, column_s, line, column});
      break;
    }

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_': {
      core::String buffer = "";
      buffer += *it;
      ++it;
      ++column;
      bool finished = it == end;
      while (!finished) {
        switch (*it) {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
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
          buffer += *it;
          ++it;
          ++column;
          finished = it == end;
          break;
        }

        default: {
          finished = true;
        }
        }
      }
      static const core::Map<core::String, ast::tokens::TokenType> keywords = {
          {"alias", ALIAS},       {"bit", BIT},         {"bitcast", BITCAST},
          {"break", BREAK},       {"case", CASE},       {"cast", CAST},
          {"continue", CONTINUE}, {"default", DEFAULT}, {"do", DO},
          {"else", ELSE},         {"false", FALSE},     {"for", FOR},
          {"fun", FUN},           {"if", IF},           {"operator", OPERATOR},
          {"other", OTHER},       {"return", RETURN},   {"sizeof", SIZEOF},
          {"struct", STRUCT},     {"switch", SWITCH},   {"syscall", SYSCALL},
          {"this", THIS},         {"true", TRUE},       {"union", UNION},
          {"var", VAR},           {"void", VOID},       {"while", WHILE}};

      if (keywords.contains(buffer)) {
        result.pushBack((ast::tokens::Token){keywords[buffer], line_s, column_s,
                                             line, column});
      } else {
        result.pushBack(
            (ast::tokens::Token){ID, buffer, line_s, column_s, line, column});
      }
      break;
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
      u64 buffer = (*it) - '0';
      ++it;
      ++column;
      bool finished = it == end;
      while (!finished) {
        switch (*it) {
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
          buffer = buffer * 10 + (*it - '0');
          ++it;
          ++column;
          finished = it == end;
          break;
        }

        default: {
          finished = true;
          break;
        }
        }
      }
      result.pushBack(
          (ast::tokens::Token){INT, buffer, line_s, column_s, line, column});
      break;
    }

    case '\'': {
      ++it;
      ++column;
      if (it == end) {
        return lexingError("End-of-file in char litteral.", line, column);
      }
      char c;
      switch (*it) {
      case '\\': {
        ++it;
        ++column;
        if (it == end) {
          return lexingError("End-of-file in escaped char litteral.", line,
                             column);
        }
        auto escaped = escape(*it);
        if (escaped.failed()) {
          return lexingError(escaped.getMessage(), line, column);
        }
        ++it;
        ++column;
        if (it == end) {
          return lexingError("End-of-file in escaped char litteral.", line,
                             column);
        }
        c = escaped.getValue();
        break;
      }

      case '\'': {
        return lexingError("Empty char litteral.", line, column);
      }

      default: {
        c = *it;
        ++it;
        ++column;
        if (it == end) {
          return lexingError("End-of-file in escaped char litteral.", line,
                             column);
        }
        break;
      }
      }
      if (*it != '\'') {
        return lexingError("Unterminated char litteral.", line, column);
      }
      ++it;
      ++column;
      result.pushBack(
          (ast::tokens::Token){CHAR, c, line_s, column_s, line, column});
    }

    case '"': {
      ++it;
      ++column;
      core::String buffer = "";
      bool escapeNext = false;
      bool finished = false;
      while (!finished) {
        if (it == end) {
          return lexingError("End-of-file in string litteral.", line, column);
        }
        if (escapeNext) {
          auto escaped = escape(*it);
          if (escaped.failed()) {
            return lexingError(escaped.getMessage(), line, column);
          }
          buffer = buffer + core::String::of(escaped.getValue());
          ++it;
          ++column;
          escapeNext = false;
        } else {
          switch (*it) {
          case '\\': {
            ++it;
            ++column;
            escapeNext = true;
            break;
          }

          case '"': {
            ++it;
            ++column;
            finished = true;
            break;
          }

          case '\n': {
            ++it;
            ++line;
            column = 1;
            buffer = buffer + "\n";
            break;
          }

          default: {
            buffer = buffer + core::String::of(*it);
            ++it;
            ++column;
            break;
          }
          }
        }
      }
      result.pushBack(
          (ast::tokens::Token){STRING, buffer, line_s, column_s, line, column});
      break;
    }

    case ' ':
    case '\r':
    case '\t': {
      ++it;
      ++column;
      break;
    }

    case '\n': {
      ++it;
      ++line;
      column = 1;
      break;
    }

    case 0: {
      return core::LogicalResult<core::Vector<ast::tokens::Token>>::success(
          result);
    }

    default: {
      core::String c = "";
      c += *it;
      return lexingError("Unexpected character \'" + c + "\'.", line, column);
    }
    }
  }
  return core::LogicalResult<core::Vector<ast::tokens::Token>>::success(result);
}

core::LogicalResult<core::Vector<ast::tokens::Token>>
lex(core::String content) {
  return lex(content.begin(), content.end());
}

core::LogicalResult<core::Vector<ast::tokens::Token>>
lexFile(core::String file) {
  auto content = core::String::fromFile(file);
  return lex(content);
}

}; // namespace compiler::frontend