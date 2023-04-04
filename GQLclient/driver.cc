#include <iostream>

#include "driver.hpp"

int main()
{
  FlexLexer *lexer = new yyFlexLexer;
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