#pragma once

#include <iostream>
#include <numeric>
#include <unordered_map>

#include "parser.tab.hh"
#include <FlexLexer.h>

#include "Node.hpp"

namespace yy
{
    class Driver
    {
        FlexLexer *plex_;
        std::vector<Node> *queryList = new std::vector<Node>;

        const std::unordered_map<std::string, Cmp> table = {{"GT", Cmp::GT}, {"gt", Cmp::GT}, {"GE", Cmp::GE}, {"ge", Cmp::GE}, {"LT", Cmp::LT}, {"lt", Cmp::LT}, {"LE", Cmp::LE}, {"le", Cmp::LE}, {"EQ", Cmp::EQ}, {"eq", Cmp::EQ}, {"IN", Cmp::IN}, {"in", Cmp::IN}};

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
            case yy::parser::token_type::CMP:
            case yy::parser::token_type::EQ:
            case yy::parser::token_type::IN:
                yylval->as<Cmp>() = table.find(plex_->YYText())->second;
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

        void insert(Node &query)
        {
            queryList->push_back(query);
        }
    };
}