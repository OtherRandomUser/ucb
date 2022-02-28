#include <ucb/frontend/lexer.hpp>

#include <iostream>
#include <fstream>
#include <unordered_map>

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
            || c == ';';
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

            if (c >= 0 && c <= 9)
                return _read_numeric_tk();

            if (is_op(c))
            {
                _cursor++;
                return _build_tk(static_cast<TokenType>(c));
            }

            if (c == '@')
            {
                _eat_id();
                return _build_tk(TokenType::ID_GLOBAL);
            }

            if (c == '%')
            {
                _eat_id();
                return _build_tk(TokenType::ID_LOCAL);
            }

            auto id = _eat_id();
            auto ty = KEYWORD_TBL.find(id);

            if (ty != KEYWORD_TBL.end())
                return _build_tk(ty->second);
            else
                return _build_tk(TokenType::ID_OTHER);
        }

        return {
            .ty = TokenType::END_OF_FILE,
            .row = _row,
            .col = _col
        };
    }

    Token Lexer::_read_whitespace_tk()
    {
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

        return _build_tk(TokenType::WHITESPACE);
    }

    Token Lexer::_read_comment_ln_tk()
    {
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

        std::cerr << "Lexer Error: expected new line but found end of file" << std::endl;
        abort();
    }

    Token Lexer::_read_str_tk()
    {
        while (_cursor != _src.end())
        {
            auto c = *_cursor;

            _cursor++;
            _col++;

            if (c == '"')
                return _build_tk(TokenType::LT_STRING);
        }

        std::cerr << "Lexer Error: expected '\"' but found end of file" << std::endl;
        abort();
    }

    Token Lexer::_read_numeric_tk()
    {
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
                        return _build_tk(TokenType::LT_FLOAT);
                }
            }

            if (c < '0' || c > '9')
                break;
        }

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
