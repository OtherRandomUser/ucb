#pragma once

#include <string>

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
        OP_CMP   = 413,
        OP_BR    = 414,
        OP_BRC   = 415,
        OP_ALLOC = 416,
        OP_LOAD  = 417,
        OP_STORE = 418,
        OP_CP    = 419,
        OP_CALL  = 420,
        OP_RET   = 421,

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

    constexpr bool tk_is_transient(Token tk)
    {
        return tk.ty >= TokenType::WHITESPACE
            && tk.ty <= TokenType::COMMENT_LINE;
    }

    constexpr bool tk_is_type(Token tk)
    {
        return tk.ty >= TokenType::TY_VOID
            && tk.ty <  TokenType::TG_UNIT;
    }

    constexpr bool tk_is_literal(Token tk)
    {
        return tk.ty >= TokenType::LT_INT
            && tk.ty <= TokenType::LT_STRING;
    }

    constexpr bool tk_is_bin_op(Token tk)
    {
        return tk.ty == TokenType::OP_ADD
            || tk.ty == TokenType::OP_SUB
            || tk.ty == TokenType::OP_MUL
            || tk.ty == TokenType::OP_DIV
            || tk.ty == TokenType::OP_REM
            || tk.ty == TokenType::OP_AND
            || tk.ty == TokenType::OP_OR
            || tk.ty == TokenType::OP_XOR
            || tk.ty == TokenType::OP_SHL
            || tk.ty == TokenType::OP_SHR;
    }
};
