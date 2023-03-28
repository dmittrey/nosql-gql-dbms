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

%token <Command> INSERT
%token <Command> DELETE
%token <Command> SELECT
%token <Command> UPDATE

%token <Cmp> CMP
%token <Cmp> EQ
%token <Cmp> IN

%nterm <FieldNode> field 
%nterm <FieldListNode> field_list
%nterm <ConditionNode> condition
%nterm <ConditionBodyNode> condition_body

%nterm <PropertyNode> property 
%nterm <PropertyListNode> property_list 
%nterm <EntityNode> entity
%nterm <EntityBodyNode> entity_body

%nterm <WordListNode> word_list
%nterm <ReprNode> repr

%nterm <InsertQueryNode> insert_query
%nterm <SelectQueryNode> select_query
%nterm <DeleteQueryNode> delete_query
%nterm <UpdateQueryNode> update_query

%start repr

%%

query_list: insert_query                                { driver->insert($1); }
  | query_list insert_query                             { driver->insert($2); }
  | select_query                                        { driver->insert($1); }
  | query_list select_query                             { driver->insert($2); }
  | delete_query                                        { driver->insert($1); }
  | query_list delete_query                             { driver->insert($2); }
  | update_query                                        { driver->insert($1); }
  | query_list update_query                             { driver->insert($2); }
;

insert_query: INSERT LB entity_body repr RB             { $$ = InsertQueryNode{$1, $3, $4}; }
;
select_query: SELECT LB condition_body repr RB          { $$ = SelectQueryNode{$1, $3, $4}; }
;
delete_query: DELETE LB condition_body repr RB          { $$ = DeleteQueryNode{$1, $3, $4}; }
;
update_query: UPDATE LB condition_body entity repr RB   { $$ = UpdateQueryNode{$1, $3, $4, $5}; }
;

entity_body: WORD LP entity RP                { $$ = EntityBodyNode{$1, $3}; }
;

entity: LB field_list RB                      { $$ = EntityNode{$2}; }
;

condition_body: WORD LP condition RP          { $$ = ConditionBodyNode{$1, $3}; }
;

condition: LB property_list RB                { $$ = ConditionNode{$2}; }
;

repr: LB word_list RB                         { $$ = ReprNode{$2}; }
  | LB RB                                     { $$ = ReprNode{};   }
;

word_list: WORD                               { $$ = WordListNode{}; $$.add(FieldNode{FIELD, $1}); }
  | word_list WORD                            { $$.add(FieldNode{FIELD, $2}); }
;

property_list: property                       { $$.add($1); }
  | property_list property                    { $$.add($2); }
;

field_list: field                             { $$.add($1); }
  | field_list field                          { $$.add($2); }
;

property: WORD IN STRING                      { $$ = StringPropertyNode{$1, $2, $3}; }
  | WORD EQ STRING                            { $$ = StringPropertyNode{$1, $2, $3}; }
  | WORD CMP INT                              { $$ = IntPropertyNode{$1, $2, $3}; }
  | WORD EQ INT                               { $$ = IntPropertyNode{$1, $2, $3}; }
  | WORD CMP DOUBLE                           { $$ = DoublePropertyNode{$1, $2, $3}; }
  | WORD EQ DOUBLE                            { $$ = DoublePropertyNode{$1, $2, $3}; }
  | WORD CMP BOOL                             { $$ = BoolPropertyNode{$1, $2, $3}; }
  | WORD EQ BOOL                              { $$ = BoolPropertyNode{$1, $2, $3}; }
  | WORD COLON condition                      { /* */ }
;

field: WORD COLON STRING                      { $$ = StringFieldNode{$1, $3}; }
  | WORD COLON INT                            { $$ = IntFieldNode{$1, $3}; }
  | WORD COLON DOUBLE                         { $$ = DoubleFieldNode{$1, $3}; }
  | WORD COLON BOOL                           { $$ = BoolFieldNode{$1, $3}; }
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