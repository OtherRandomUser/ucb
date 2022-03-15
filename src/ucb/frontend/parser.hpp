#pragma once

#include <ucb/core/config.hpp>

#include <memory>

#include <ucb/core/ir/compile-unit.hpp>
#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/frontend/lexer.hpp>

namespace ucb::frontend
{
    class Parser
    {
    public:
        Parser(std::string filename, std::shared_ptr<CompileUnit> compile_unit, bool debug_lexer, bool debug_parser):
            _filename(std::move(filename)),
            _compile_unit(compile_unit),
            _lex(_filename, debug_lexer),
            _debug(debug_parser)
        {
        }

        bool parse_unit();

    private:
        std::string _filename;
        std::shared_ptr<CompileUnit> _compile_unit;
        std::shared_ptr<Procedure> _proc;
        Lexer _lex;
        Token _cur;
        bool _debug;

        bool _parse_def();
        bool _parse_global();
        bool _parse_func();
        bool _parse_param_defs();
        bool _parse_statement();
        bool _parse_label();
        bool _parse_assign_statement();
        bool _parse_store();
        bool _parse_br();
        bool _parse_brc();
        bool _parse_ret();
        bool _parse_bin_op();
        bool _parse_unnary_op();
        bool _parse_cast();
        bool _parse_alloc();
        bool _parse_load();
        bool _parse_call();
        bool _parse_cmp();

        bool _parse_ty(Type *ty);
        bool _parse_opnd(Operand *op);

        Token _bump();
    };
};
