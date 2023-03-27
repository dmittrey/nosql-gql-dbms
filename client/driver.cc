#include "driver.hpp"

int main() {
  FlexLexer *lexer = new yyFlexLexer;
  yy::Driver driver(lexer);
  driver.parse();
  delete lexer;

  return 0;
}