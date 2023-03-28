#pragma once

#include <iostream>
#include <numeric>

#include "parser.tab.hh"
#include <FlexLexer.h>

#include "INode.hpp"

namespace yy
{

    class Driver
    {
        FlexLexer *plex_;

    public:
        Driver(FlexLexer *plex) : plex_(plex) {}

        parser::token_type yylex(parser::semantic_type *yylval)
        {
            parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());

            switch (tt)
            {
            case yy::parser::token_type::WORD:
            case yy::parser::token_type::STRING:
            case yy::parser::token_type::INSERT:
            case yy::parser::token_type::DELETE:
            case yy::parser::token_type::SELECT:
            case yy::parser::token_type::UPDATE:
                yylval->as<std::string>() = plex_->YYText();
                break;
            case yy::parser::token_type::INT:
                yylval->as<int>() = std::stoi(plex_->YYText());
                break;
            case yy::parser::token_type::DOUBLE:
                yylval->as<double>() = std::stod(plex_->YYText());
                break;
            case yy::parser::token_type::BOOL:
                yylval->as<bool>() = (strcmp(plex_->YYText(), "true") == 0 || strcmp(plex_->YYText(), "TRUE") == 0);
                break;
            default:
                break;
            }

            return tt;
        }

        bool parse()
        {
            parser parser(this);
            return parser.parse();
        }
    };

}