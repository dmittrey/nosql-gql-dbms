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
            return static_cast<parser::token_type>(plex_->yylex());
        }

        bool parse()
        {
            parser parser(this);
            return parser.parse();
        }
    };

}