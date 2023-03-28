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

#include "Node.hpp"

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

%token <Cmp> CMP
%token <Cmp> EQ
%token <Cmp> IN

%nterm <FieldNode> field
%nterm <ListNode<FieldNode>> field_list
%nterm <ListNode<FieldNode>> entity
%nterm <EntityBodyNode> entity_body

%nterm <PropertyNode> property  
%nterm <ListNode<PropertyNode>> property_list conditional
%nterm <ConditionalBodyNode> conditional_body

%nterm <WordListNode> word_list repr

%start query_list

%%

query_list: query                             { std::cout << "Query list created!" << std::endl; }
  | query_list query                          { std::cout << "Query list created!" << std::endl; }
;

query: INSERT LB entity_body repr RB          { std::cout << "Insert query created!" << std::endl; }
  | SELECT LB conditional_body repr RB        { std::cout << "Select query created!" << std::endl; }
  | DELETE LB conditional_body repr RB        { std::cout << "Delete query created!" << std::endl; }
  | UPDATE LB conditional_body entity repr RB { std::cout << "Update query created!" << std::endl; }
;

entity_body: WORD LP entity RP                { $$ = EntityBodyNode{$3, $1}; }
;

entity: LB field_list RB                      { $$ = $2; }
;

conditional_body: WORD LP conditional RP      { $$ = ConditionalBodyNode{$3, $1}; }
;

conditional: LB property_list RB              { $$ = $2; }
;

repr: LB word_list RB                         { $$ = $2; }
  | LB RB                                     { /* */ }
;

word_list: WORD                               { $$.collection.push_back($1); }
  | word_list WORD                            { $$.collection.push_back($2); }
;

property_list: property                       { $$.add_son($1); }
  | property_list property                    { $$.add_son($2); }
;

field_list: field                             { $$.add_son($1); }
  | field_list field                          { $$.add_son($2); }
;

property: WORD IN STRING                      { $$ = PropertyNode{$1, FieldType::STRING, $2}; }
  | WORD EQ STRING                            { $$ = PropertyNode{$1, FieldType::STRING, $2}; }
  | WORD CMP INT                              { $$ = PropertyNode{$1, FieldType::INT32, $2}; }
  | WORD EQ INT                               { $$ = PropertyNode{$1, FieldType::INT32, $2}; }
  | WORD CMP DOUBLE                           { $$ = PropertyNode{$1, FieldType::DOUBLE, $2}; }
  | WORD EQ DOUBLE                            { $$ = PropertyNode{$1, FieldType::DOUBLE, $2}; }
  | WORD CMP BOOL                             { $$ = PropertyNode{$1, FieldType::BOOL, $2}; }
  | WORD EQ BOOL                              { $$ = PropertyNode{$1, FieldType::BOOL, $2}; }
  | WORD COLON conditional                    { /* */ }
;

field: WORD COLON STRING                      { $$ = FieldNode{$1, $3}; }
  | WORD COLON INT                            { $$ = FieldNode{$1, $3}; }
  | WORD COLON DOUBLE                         { $$ = FieldNode{$1, $3}; }
  | WORD COLON BOOL                           { $$ = FieldNode{$1, $3}; }
  | WORD COLON entity                         { /* */ }
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