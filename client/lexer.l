%option c++ case-insensitive

%{

#include "parser.tab.hh"

%}

WS      [ \t\n\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
WORD    [A-Za-z][A-Za-z0-9_]*

%%

{WS}                                        /* skip blanks and tabs */
"IN"                                        return yy::parser::token_type::IN;
"EQ"                                        return yy::parser::token_type::EQ;
"MORE" | "LESS"                             return yy::parser::token_type::CMP;
"{"                                         return yy::parser::token_type::LB;
"}"                                         return yy::parser::token_type::RB;
"("                                         return yy::parser::token_type::LP;
")"                                         return yy::parser::token_type::RP;
"INSERT" | "UPDATE" | "SELECT" | "DELETE"   return yy::parser::token_type::COMMAND;
"TRUE" | "FALSE"                            return yy::parser::token_type::BOOL;
\"{WORD}\"                                  return yy::parser::token_type::STRING;
{WORD}                                      return yy::parser::token_type::WORD;
"^(-?)(0|([1-9][0-9]*))(\\.[0-9]+)?$"       return yy::parser::token_type::DOUBLE;
{DIGIT1}{DIGIT}*                            return yy::parser::token_type::INT;
":"                                         return yy::parser::token_type::COLON;
.			                                return yy::parser::token_type::ERR;

%%