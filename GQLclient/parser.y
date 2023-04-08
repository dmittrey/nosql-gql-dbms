%language "c++"

%defines
%define api.value.type variant
%param {yy::Driver* driver}

%code requires
{

#include "node.hpp"

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
%token <float> FLOAT
%token <bool> BOOL

%token <std::string> WORD

%token <OperationType> INSERT
%token <OperationType> DELETE
%token <OperationType> SELECT
%token <OperationType> UPDATE

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
%nterm <EntityListNode> entity_list
%nterm <EntityBodyNode> entity_body

%nterm <InsertQueryNode> insert_query
%nterm <SelectQueryNode> select_query
%nterm <DeleteQueryNode> delete_query
%nterm <UpdateQueryNode> update_query

%start query_list

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

insert_query: INSERT LB entity_body RB                   { $$ = InsertQueryNode{$1, $3}; }
;
select_query: SELECT LB condition_body RB                { $$ = SelectQueryNode{$1, $3}; }
;
delete_query: DELETE LB condition_body RB                { $$ = DeleteQueryNode{$1, $3}; }
;
update_query: UPDATE LB condition_body entity_body RB    { $$ = UpdateQueryNode{$1, $3, $4}; }
;

entity_body: WORD LP LB entity_list RB RP                     { $$ = EntityBodyNode{$1, $4}; }
;

entity_list: entity                                           { $$.add($1); }
| entity_list entity                                          { $$ = $1; $$.add($2); }
; 

entity: WORD COLON LB field_list RB                           { $$ = EntityNode{$1, $4}; }
| field_list                                                  { $$ = EntityNode{$1}; }
;

condition_body: WORD LP condition RP                          { $$ = ConditionBodyNode{$1, $3}; }
;

condition: LB property_list RB                                { $$ = ConditionNode{$2}; }
;

property_list: property                                       { $$.add($1); }
| property_list property                                      { $$ = $1; $$.add($2); }
;

field_list: field                                             { $$.add($1); }
| field_list field                                            { $$ = $1; $$.add($2); }
;

property: WORD IN STRING                                      { $$ = PropertyNode{$1, $2, $3}; }
| WORD EQ STRING                                              { $$ = PropertyNode{$1, $2, $3}; }
| WORD CMP INT                                                { $$ = PropertyNode{$1, $2, $3}; }
| WORD EQ INT                                                 { $$ = PropertyNode{$1, $2, $3}; }
| WORD CMP FLOAT                                             { $$ = PropertyNode{$1, $2, $3}; }
| WORD EQ FLOAT                                              { $$ = PropertyNode{$1, $2, $3}; }
| WORD CMP BOOL                                               { $$ = PropertyNode{$1, $2, $3}; }
| WORD EQ BOOL                                                { $$ = PropertyNode{$1, $2, $3}; }
;

field: WORD COLON STRING                                      { $$ = FieldNode{$1, $3}; }
| WORD COLON INT                                              { $$ = FieldNode{$1, $3}; }
| WORD COLON FLOAT                                           { $$ = FieldNode{$1, $3}; }
| WORD COLON BOOL                                             { $$ = FieldNode{$1, $3}; }
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