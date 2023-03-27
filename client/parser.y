 /* ------------------------------------------------------------------------- **
 *
 *  Grammar
 *
 *  field := WORD COLON STRING
 *   | WORD COLON NUMBER
 *  
 *  field_list := field
 *   | field_list field
 *
 *  word_list := WORD
 *   | word_list WORD
 *
 *  repr := LB word_list RB
 *
 *  entity := LB field_list RB
 *
 *  body := WORD LP entity RP
 *
 *  query := COMMAND LB body repr RB
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
  IN
  EQ
  CMP
  LB
  RB
  LP
  RP
  COMMAND
  COLON
  ERR
;

%token <int> INT
%token <double> DOUBLE
%token <bool> BOOL
%token <std::string> STRING
%token <std::string> WORD

%start query

%%

query: COMMAND LB body repr RB               { /* Nothing yet */ }
;

body: WORD LP entity RP                      { /* Nothing yet */ }
;

entity: LB field_list RB                     { /* Nothing yet */ }
;

repr: LB word_list RB                        { /* Nothing yet */ }
;

word_list: WORD                              { /* Nothing yet */ }
  | word_list WORD                           { /* Nothing yet */ }
;

field_list: field                            { /* Nothing yet */ }
  | field_list field                         { /* Nothing yet */ }
;

field: WORD COLON STRING                     { /* Nothing yet */ }
  | WORD COLON INT                           { /* Nothing yet */ }
  | WORD COLON BOOL                          { /* Nothing yet */ }
  | WORD COLON DOUBLE                        { /* Nothing yet */ }
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