#pragma once

#include <ucb/core/config.hpp>

#include <ucb/frontend/lexer.hpp>

namespace ucb::frontend
{
    class Parser
    {
    public:
        Parser(std::string filename, bool debug_lexer, bool debug_parser):
            _lex(std::move(filename), debug_lexer),
            _debug(debug_parser)
        {
        }

        bool parse_unit();

    private:
        Lexer _lex;
        bool _debug;
    };
};
