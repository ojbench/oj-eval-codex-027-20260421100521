#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "utils/Error.hpp"

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    try {
      TokenStream tokens = lexer.tokenize(line);
      // Support meta-commands without line numbers
      const Token* first = tokens.peek();
      if (first && first->type == TokenType::RUN) {
        tokens.get();
        program.run();
        continue;
      }
      if (first && first->type == TokenType::LIST) {
        tokens.get();
        program.list();
        continue;
      }
      if (first && first->type == TokenType::CLEAR) {
        tokens.get();
        program.clear();
        continue;
      }
      if (first && first->type == TokenType::INDENT) {
        tokens.get();
        program.execute(makeIndent());
        continue;
      }
      if (first && first->type == TokenType::DEDENT) {
        tokens.get();
        program.execute(makeDedent());
        continue;
      }
      if (first && first->type == TokenType::QUIT) {
        break;
      }

      auto parsed = parser.parseLine(tokens, line);
      auto lineNo = parsed.getLine();
      Statement* stmt = parsed.fetchStatement();
      if (lineNo.has_value()) {
        // program line
        if (stmt == nullptr) {
          program.removeStmt(*lineNo);
        } else {
          program.addStmt(*lineNo, stmt);
        }
      } else {
        // immediate mode
        if (stmt) {
          std::unique_ptr<Statement> guard(stmt);
          program.execute(stmt);
        }
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}
