#pragma once

#include <ucb/core/config.hpp>
#include <ucb/frontend/token.hpp>

#include <string>
#include <iostream>

namespace ucb::frontend
{
    class Lexer
    {
    public:
        Lexer(std::string filename, bool print_debug);

        Token bump();
        Token peek();
        bool is_eof();

    private:
        std::string _filename;
        std::string _src;
        bool _print_debug;

        std::string::iterator _cursor;
        std::string::iterator _head_cursor;
        long _row;
        long _head_row;
        long _col;
        long _head_col;

        Token _current;

        Token _read_token();
        Token _read_whitespace_tk();
        Token _read_comment_ln_tk();
        Token _read_str_tk();
        Token _read_numeric_tk();
        std::string _eat_id();

        std::string_view _build_lexema();
        Token _build_tk(TokenType ty);
    };
}
