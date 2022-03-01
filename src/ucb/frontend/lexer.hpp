#pragma once

#include <ucb/core/config.hpp>

#include <string>
#include <iostream>

namespace ucb::frontend
{
    enum TokenType : int
    {
        ID_LOCAL      = 201,
        ID_GLOBAL     = 202,
        ID_OTHER      = 203,
        WHITESPACE    = 204,
        LINE_BREAK    = 205,
        END_OF_FILE   = 206,
        COMMENT_LINE  = 207,
        COMMENT_BLOCK = 208,

        // literals
        LT_INT    = 301,
        LT_FLOAT  = 302,
        LT_CHAR   = 303,
        LT_STRING = 304,

        // ops
        OP_ADD   = 401,
        OP_SUB   = 402,
        OP_MUL   = 403,
        OP_DIV   = 404,
        OP_REM   = 405,
        OP_NOT   = 406,
        OP_AND   = 407,
        OP_OR    = 408,
        OP_XOR   = 409,
        OP_SHL   = 410,
        OP_SHR   = 411,
        OP_CAST  = 412,
        OP_BR    = 413,
        OP_BRC   = 414,
        OP_ALLOC = 415,
        OP_LOAD  = 416,
        OP_STORE = 417,
        OP_CP    = 418,
        OP_CALL  = 419,
        OP_RET   = 420,

        // types
        TY_VOID = 501,
        TY_BOOL = 502,
        TY_I8   = 503,
        TY_I16  = 504,
        TY_I32  = 505,
        TY_I64  = 506,
        TY_U8   = 507,
        TY_U16  = 508,
        TY_U32  = 509,
        TY_U64  = 510,
        TY_F32  = 511,
        TY_F64  = 512,

        // tags
        TG_UNIT   = 601,
        TG_TYPE   = 602,
        TG_PTR_TY = 603,
        TG_ARR_TY = 604,
        TG_PROC   = 605,
        TG_PARAM  = 606,
        TG_BLOCK  = 607,
        TG_END    = 608,
        TG_VAR    = 609,
        TG_LOC    = 610
    };

    struct Token
    {
        TokenType ty;
        long row;
        long col;
        std::string_view lexema;
    };

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
