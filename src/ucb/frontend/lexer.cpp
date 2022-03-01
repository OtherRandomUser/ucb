#include <ucb/frontend/lexer.hpp>

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <fmt/core.h>

#define DEBUG(T, ...) \
    if (_print_debug) { fmt::print(stderr, "Lexer: " T __VA_OPT__(,) __VA_ARGS__); }

#define DEBUG_LOC(T, ...) \
    if (_print_debug) { fmt::print(stderr, "Lexer: {}:{}:{} " T, _filename, _row, _col __VA_OPT__(,) __VA_ARGS__); }

#define ERROR(T, ...) \
    { \
        fmt::print(stderr, "Lexer: " T __VA_OPT__(,) __VA_ARGS__); \
        fmt::print(stderr, "\t cursor at {}:{}:{}", _filename, _head_row, _head_col); \
        abort(); \
    }

namespace ucb::frontend
{
    static std::unordered_map<std::string, TokenType> KEYWORD_TBL = {{
        {"add", TokenType::OP_ADD},
        {"sub", TokenType::OP_SUB},
        {"mul", TokenType::OP_MUL},
        {"div", TokenType::OP_DIV},
        {"rem", TokenType::OP_REM},
        {"not", TokenType::OP_NOT},
        {"and", TokenType::OP_AND},
        {"or", TokenType::OP_OR},
        {"xor", TokenType::OP_XOR},
        {"shl", TokenType::OP_SHL},
        {"shr", TokenType::OP_SHR},
        {"cast", TokenType::OP_CAST},
        {"br", TokenType::OP_BR},
        {"brc", TokenType::OP_BRC},
        {"alloc", TokenType::OP_ALLOC},
        {"load", TokenType::OP_LOAD},
        {"store", TokenType::OP_STORE},
        {"cp", TokenType::OP_CP},
        {"call", TokenType::OP_CALL},
        {"ret", TokenType::OP_RET},

        {"void", TokenType::TY_VOID},
        {"bool", TokenType::TY_BOOL},
        {"i8", TokenType::TY_I8},
        {"i16", TokenType::TY_I16},
        {"i32", TokenType::TY_I32},
        {"i64", TokenType::TY_I64},
        {"u8", TokenType::TY_U8},
        {"u16", TokenType::TY_U16},
        {"u32", TokenType::TY_U32},
        {"u64", TokenType::TY_U64},
        {"f32", TokenType::TY_F32},
        {"f64", TokenType::TY_F64},

        {"!unit", TokenType::TG_UNIT},
        {"!type", TokenType::TG_TYPE},
        {"!ptr_type", TokenType::TG_PTR_TY},
        {"!array_type", TokenType::TG_ARR_TY},
        {"!proc", TokenType::TG_PROC},
        {"!param", TokenType::TG_PARAM},
        {"!block", TokenType::TG_BLOCK},
        {"!end", TokenType::TG_END},
        {"!var", TokenType::TG_VAR},
        {"!loc", TokenType::TG_LOC},
    }};

    inline bool is_whitespace(char c)
    {
        return c == ' '
            || c == '\t'
            || c == '\n'
            || c == '\r';
    }

    inline bool is_op(char c)
    {
        return c == '^'
            || c == '{'
            || c == '}'
            || c == ','
            || c == '='
            || c == ';'
            || c == '('
            || c == ')';
    }

    Lexer::Lexer(std::string filename, bool print_debug):
        _filename(std::move(filename)),
        _print_debug(print_debug)
    {
        std::ifstream ifs(_filename);

        if (!ifs.is_open())
        {
            std::cerr << "could not open source file\"" << _filename << std::endl;
            exit(EXIT_FAILURE);
        }

        _src = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

        _cursor      = _src.begin();
        _head_cursor = _src.begin();
        _col         = 1;
        _head_col    = 1;
        _row         = 1;
        _head_row    = 1;

        _current = _read_token();
    }

    Token Lexer::bump()
    {
        auto res = _current;
        _current = _read_token();
        return res;
    }

    Token Lexer::peek()
    {
        return _current;
    }

    bool Lexer::is_eof()
    {
        return _current.ty == TokenType::END_OF_FILE;
    }

    Token Lexer::_read_token()
    {
        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            if (is_whitespace(c))
                return _read_whitespace_tk();

            if (c == ';')
                return _read_comment_ln_tk();

            if (c == '"')
                return _read_str_tk();

            if (c >= '0' && c <= '9')
                return _read_numeric_tk();

            if (is_op(c))
            {
                _cursor++;
                DEBUG_LOC("read op token with lexema '{}'", _build_lexema());
                return _build_tk(static_cast<TokenType>(c));
            }

            if (c == '@')
            {
                _eat_id();
                DEBUG_LOC("read global id token with lexema '{}'", _build_lexema());
                return _build_tk(TokenType::ID_GLOBAL);
            }

            if (c == '%')
            {
                _eat_id();
                DEBUG_LOC("read local id token with lexema '{}'", _build_lexema());
                return _build_tk(TokenType::ID_LOCAL);
            }

            auto id = _eat_id();
            auto ty = KEYWORD_TBL.find(id);

            if (ty != KEYWORD_TBL.end())
            {
                DEBUG_LOC("read keyword token with lexema '{}'", _build_lexema());
                return _build_tk(ty->second);
            }
            else
            {
                DEBUG_LOC("read other token with lexema '{}'", _build_lexema());
                return _build_tk(TokenType::ID_OTHER);
            }
        }

        return {
            .ty = TokenType::END_OF_FILE,
            .row = _row,
            .col = _col
        };
    }

    Token Lexer::_read_whitespace_tk()
    {
        DEBUG("Reading whitespace token");

        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            if (!is_whitespace(c))
                break;

            _cursor++;
            _col++;

            if (c == '\n')
            {
                _row++;
                _col = 1;
            }
        }

        DEBUG_LOC("read whitespace token");
        return _build_tk(TokenType::WHITESPACE);
    }

    Token Lexer::_read_comment_ln_tk()
    {
        DEBUG("Reading comment line token");

        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            _cursor++;
            _col++;

            if (c == '\n')
            {
                _row++;
                _col = 1;

                return _build_tk(TokenType::COMMENT_LINE);
            }
        }

        DEBUG_LOC("read comment line token with lexema '{}'", _build_lexema());
        ERROR("expected new line but found end of file");
    }

    Token Lexer::_read_str_tk()
    {
        DEBUG("Reading string literal token");

        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            _cursor++;
            _col++;

            if (c == '"')
                return _build_tk(TokenType::LT_STRING);
        }

        DEBUG_LOC("read string literal token with lexema '{}'", _build_lexema());
        ERROR("expected '\"' but found end of file");
    }

    Token Lexer::_read_numeric_tk()
    {
        DEBUG("Reading numeric token");

        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            _cursor++;
            _col++;

            if (c == '.')
            {
                while (_cursor != _src.end())
                {
                    auto d = *_cursor;

                    _cursor++;
                    _col++;

                    if (d < '0' || d > '9')
                    {
                        DEBUG_LOC("read floating point literal with lexema '{}'", _build_lexema());
                        return _build_tk(TokenType::LT_FLOAT);
                    }
                }
            }

            if (c < '0' || c > '9')
                break;
        }

        DEBUG_LOC("read integer literal with lexema '{}'", _build_lexema());
        return _build_tk(TokenType::LT_INT);
    }

    std::string Lexer::_eat_id()
    {
        auto start = _cursor;

        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            _cursor++;
            _col++;

            if (is_whitespace(c) || is_op(c))
                break;
        }

        return std::string(start, _cursor);
    }

    std::string_view Lexer::_build_lexema()
    {
        return std::string_view(_head_cursor, _cursor);
    }

    Token Lexer::_build_tk(TokenType ty)
    {
        auto row = _head_row;
        auto col = _head_col;
        std::string_view lex(_head_cursor, _cursor);

        _head_row = _row;
        _head_col = _col;
        _head_cursor = _cursor;

        return {
            .ty     = ty,
            .row    = row,
            .col    = col,
            .lexema = lex
        };
    }
}
