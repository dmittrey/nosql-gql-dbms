#include <iostream>

#include "driver.hpp"

int main(int argc, char **argv)
{
  FlexLexer *lexer = new yyFlexLexer{};
  DataBaseClient *client;
  if (argc == 1)
    client = new DataBaseClient(5432);
  if (argc == 2)
    client = new DataBaseClient(std::stoi(argv[1]));
  else if (argc == 3)
    client = new DataBaseClient(std::stoi(argv[1]), argv[2]);
  else
    return 1;

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