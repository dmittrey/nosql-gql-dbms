%option c++ case-insensitive noyywrap

%{

#include "parser.tab.hh"

%}

WS      [ \t\n\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
WORD    [A-Za-z][A-Za-z0-9_]*

%%

{WS}                                        /* skip blanks and tabs */

\"{WORD}\"                                  return yy::parser::token_type::STRING;
(-?)(0|{DIGIT1}{DIGIT}*)(\.{DIGIT}+)        return yy::parser::token_type::DOUBLE;
(-?){DIGIT1}{DIGIT}*                        return yy::parser::token_type::INT;
true|false                                  return yy::parser::token_type::BOOL;

INSERT|UPDATE|DELETE|SELECT                 return yy::parser::token_type::COMMAND;
{WORD}                                      return yy::parser::token_type::WORD;

"{"                                         return yy::parser::token_type::LB;
"}"                                         return yy::parser::token_type::RB;
"("                                         return yy::parser::token_type::LP;
")"                                         return yy::parser::token_type::RP;
":"                                         return yy::parser::token_type::COLON;

.			                                return yy::parser::token_type::ERR;

%%