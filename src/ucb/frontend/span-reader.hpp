#pragma once

#include <ucb/core/config.hpp>

#include <string>
#include <string_view>

namespace ucb::frontend
{
    class Span
    {
    public:
        Span(
            short col, short row,
            std::string_view lexema,
            std::string_view row_txt
        ):
            _col{col},
            _row{row},
            _lexema{lexema},
            _row_txt{row_txt}
        {
        }

        short col() const { return _col; }
        short row() const { return _row; }

        const std::string_view lexema() const { return _lexema; }
        const std::string_view row_txt() const { return _row_txt; }

    private:
        short _col;
        short _row;

        std::string_view _lexema;
        std::string_view _row_txt;
    };

    class SpanReader
    {
    public:
        SpanReader(std::string filename);

        bool is_open() const { return _is_open; }
        bool is_eof() const { return _cursor == _txt.length(); }

        char advance_cursor();

        Span read_span();
        void discard_span();

    private:
        bool _is_open;
        std::string _filename;
        std::string _txt;

        int _cursor;
        int _current_row;
        short _cursor_row;
        short _cursor_col;

        int _row_head;
        short _row;

        int _span_head;
        short _col;
    };
}
