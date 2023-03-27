%option c++ noyywrap case-insensitive

%{

#include "parser.tab.hh"

%}

WS      [ \t\n\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
WORD    [A-Za-z][A-Za-z0-9_]*

%%

{WS}                                /* skip blanks and tabs */
"EQ"                                return yy::parser::token_type::EQ;
"MORE"                              return yy::parser::token_type::MORE;
"LESS"                              return yy::parser::token_type::LESS;
"{"                                 return yy::parser::token_type::LB;
"}"                                 return yy::parser::token_type::RB;
"INSERT"                            return yy::parser::token_type::INSERT; 
"UPDATE"                            return yy::parser::token_type::UPDATE; 
"SELECT"                            return yy::parser::token_type::SELECT; 
"DELETE"                            return yy::parser::token_type::DELETE;
\"{WORD}\"                          return yy::parser::token_type::STRING;
{WORD}                              return yy::parser::token_type::WORD;
{DIGIT1}{DIGIT}*                    return yy::parser::token_type::NUMBER;
.			                        return yy::parser::token_type::ERR;

%%

// nothing