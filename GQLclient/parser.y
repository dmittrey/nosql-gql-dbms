 /* ------------------------------------------------------------------------- **
 *  Grammar
 *
 *  query_list := query 
 *   | query_list query
 *
 *  query := INSERT LB entity_body repr RB
 *   | SELECT LB conditional_body repr RB
 *   | DELETE LB conditional_body repr RB
 *   | UPDATE LB conditional_body entity repr RB
 *
 *  entity_body := WORD LP entity RP
 *  entity := LB field_list RB
 *
 *  conditional_body := WORD LP conditional RP
 *  conditional := LB property_list RB
 *
 *  repr := LB word_list RB
 *   | LB RB
 *
 *  word_list := WORD
 *   | word_list WORD
 *
 *  property_list := property
 *   | property_list property
 *
 *  field_list := field
 *   | field_list field
 *
 *  property := WORD IN STRING
 *   | WORD EQ STRING
 *   | WORD CMP INT 
 *   | WORD EQ INT
 *   | WORD CMP DOUBLE 
 *   | WORD EQ DOUBLE
 *   | WORD CMP BOOL
 *   | WORD EQ BOOL
 *   | WORD COLON conditional
 *
 *  field := WORD COLON STRING
 *   | WORD COLON INT
 *   | WORD COLON DOUBLE
 *   | WORD COLON BOOL
 *   | WORD COLON entity
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

%token <std::string> INSERT
%token <std::string> DELETE
%token <std::string> SELECT
%token <std::string> UPDATE

%token CMP
%token EQ
%token IN

%start query_list

%%

query_list: query                            { std::cout << "Query list created!" << std::endl; }
  | query_list query                         { std::cout << "Query list created!" << std::endl; }
;

query: INSERT LB entity_body repr RB       { std::cout << "Insert query created!" << std::endl; }
  | SELECT LB conditional_body repr RB        { std::cout << "Select query created!" << std::endl; }
  | DELETE LB conditional_body repr RB        { std::cout << "Delete query created!" << std::endl; }
  | UPDATE LB conditional_body entity repr RB { std::cout << "Update query created!" << std::endl; }
;

entity_body: WORD LP entity RP               { std::cout << "Entity Body created!" << std::endl; }
;

entity: LB field_list RB                     { std::cout << "Entity created!" << std::endl; }
;

conditional_body: WORD LP conditional RP     { std::cout << "Conditional body created!" << std::endl; }
;

conditional: LB property_list RB             { std::cout << "Conditional created!" << std::endl; }
;

repr: LB word_list RB                        { std::cout << "Representation created!" << std::endl; }
  | LB RB                                    { std::cout << "Representation created!" << std::endl; }
;

word_list: WORD                              { std::cout << "WordList: " << $1 << std::endl; }
  | word_list WORD                           { std::cout << "Field: " << $2 << std::endl; }
;

property_list: property                    { std::cout << "Property list created!" << std::endl; }
  | property_list property                   { std::cout << "Property list created!" << std::endl; }
;

field_list: field                            { std::cout << "Field list created!" << std::endl; }
  | field_list field                         { std::cout << "Field list created!" << std::endl; }
;

property: WORD IN STRING                   { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD EQ STRING                           { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD CMP INT                             { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD EQ INT                              { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD CMP DOUBLE                          { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD EQ DOUBLE                           { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD CMP BOOL                            { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD EQ BOOL                             { std::cout << "Property: " << $1 << "Val: " << $3 << std::endl; }
  | WORD COLON conditional                   { std::cout << "Property: " << $1 << std::endl; }
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