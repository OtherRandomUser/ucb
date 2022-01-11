#include <ucb/frontend/lexer.hpp>

#include <fstream>
#include <sstream>

namespace ucb::frontend
{
        SpanReader::SpanReader(std::string filename):
            _filename(std::move(filename))
        {
            std::ifstream is(_filename);
            _is_open = is.is_open();

            std::stringstream ss;
            ss << is.rdbuf();

            _txt = ss.str();

            _cursor = 0;
            _current_row = 0;
            _cursor_row = 1;
            _cursor_col = 1;

            _row_head = _cursor;
            _row = _cursor_row;

            _span_head = _cursor;
            _col = _cursor_col;
        }

        char SpanReader::advance_cursor()
        {
            auto c = _txt[_cursor];
            _cursor++;
            _cursor_col++;

            if (c == '\n')
            {
                _cursor_row++;
                _current_row = _cursor;
                _cursor_col = 1;
            }

            return c;
        }

        Span SpanReader::read_span()
        {
            std::string_view span(&_txt[_span_head], _cursor - _span_head);
            std::string_view row(&_txt[_row_head], _cursor - _row_head);
            Span s(_col, _row, span, row);

            discard_span();

            return s;
        }

        void SpanReader::discard_span()
        {
            int _cursor;
            short _cursor_row;
            short _cursor_col;

            int _row_head;
            short _row;

            _span_head = _cursor;
            _row_head = _current_row;
            _col = _cursor_col;
            _row = _cursor_row;
        }
}
