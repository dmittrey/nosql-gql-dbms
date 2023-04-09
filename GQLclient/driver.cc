#include <iostream>

#include "driver.hpp"

int main()
{
  // std::ifstream file("/Users/dmitry/Desktop/low-level-programming/GQLclient/tests/select.dat");
  // FlexLexer *lexer = new yyFlexLexer{&file};
  FlexLexer *lexer = new yyFlexLexer{};
  DataBaseClient *client = new DataBaseClient(5433);
  if (client->Ping() == false)
  {
    delete lexer;
    return 0;
  }

  yy::Driver driver(lexer, client);
  driver.parse();
  delete lexer;
  return 0;
}