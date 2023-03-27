 /* ------------------------------------------------------------------------- **
 *
 *  Grammar
 *
 * ------------------------------------------------------------------------- */

%language "c++"

%defines
%define api.value.type variant
%param {yy::Driver* driver}

%code requires
{

// forward decl of argument to parser
namespace yy { class Driver; }

}

%code
{

#include "driver.hpp"

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         Driver* driver);
}

}

%token
  EQ
  MORE
  LESS
  LB
  RB
  INSERT
  UPDATE
  SELECT
  DELETE
  ERR
;

%token <int> NUMBER
%token <std::string> STRING
%token <std::string> WORD

%start program

%%

program: NUMBER               { /* Nothing yet */ }
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         Driver* driver)
{
  return driver->yylex(yylval);
}

void parser::error(const std::string&){}
}