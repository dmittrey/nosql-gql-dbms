 /* ------------------------------------------------------------------------- **
 *
 *  Grammar
 *
 *  query_list := query 
 *   | query_list query
 *
 *  query := COMMAND LB body repr RB
 *
 *  body := WORD LP entity RP
 *
 *  entity := LB field_list RB
 *
 *  repr := LB word_list RB
 *
 *  word_list := WORD
 *   | word_list WORD
 *
 *  field_list := field
 *   | field_list field
 *
 *  field := WORD COLON STRING
 *   | WORD COLON INT
 *   | WORD COLON DOUBLE
 *   | WORD COLON BOOL
 *   | WORD COLON entity
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


%token COLON
%token ERR
%token LB
%token RB
%token LP
%token RP

%token <int> INT
%token <std::string> STRING
%token <double> DOUBLE
%token <bool> BOOL

%token <std::string> WORD
%token <std::string> COMMAND

%start query_list

%%

query_list: query                            { std::cout << "Query list created!" << std::endl; }
  | query_list query                         { std::cout << "Query list created!" << std::endl; }
;

query: COMMAND LB body repr RB               { std::cout << "Query created!" << std::endl; }
;

body: WORD LP entity RP                      { std::cout << "Body created!" << std::endl; }
;

repr: LB word_list RB                        { std::cout << "Representation created!" << std::endl; }
;

entity: LB field_list RB                     { std::cout << "Entity created!" << std::endl; }
;

field_list: field                            { std::cout << "Field list created!" << std::endl; }
  | field_list field                         { std::cout << "Field list created!" << std::endl; }
;

word_list: WORD                              { std::cout << "WordList: " << $1 << std::endl; }
  | word_list WORD                           { std::cout << "Field: " << $2 << std::endl; }
;

field: WORD COLON STRING                     { std::cout << "Field: " << $1 << "\tVal: " << $3 << std::endl; }
  | WORD COLON INT                           { std::cout << "Field: " << $1 << "\tVal: " << $3 << std::endl; }
  | WORD COLON DOUBLE                        { std::cout << "Field: " << $1 << "\tVal: " << $3 << std::endl; }
  | WORD COLON BOOL                          { std::cout << "Field: " << $1 << "\tVal: " << $3 << std::endl; }
  | WORD COLON entity                        { std::cout << "Field: " << $1 << "\tVal: entity" << std::endl; }
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