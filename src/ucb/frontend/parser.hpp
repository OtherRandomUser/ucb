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
            _proc{nullptr},
            _bblock{nullptr},
            _lex(_filename, debug_lexer),
            _debug(debug_parser)
        {
        }

        bool parse_unit();

    private:
        std::string _filename;
        std::shared_ptr<CompileUnit> _compile_unit;
        std::shared_ptr<Procedure> _proc;
        BasicBlock *_bblock;
        Lexer _lex;
        Token _cur;
        bool _debug;

        bool _parse_def();
        bool _parse_global();
        bool _parse_func(TypeID ret_ty, std::string id);
        bool _parse_param_defs(TypeID ret_ty, ProcSignature& sig);
        bool _parse_statement();
        bool _parse_label();
        bool _parse_assign_statement();
        bool _parse_store();
        bool _parse_br();
        bool _parse_brc();
        bool _parse_ret();
        bool _parse_bin_op(std::string def_id);
        bool _parse_unnary_op(std::string def_id);
        bool _parse_cast(std::string def_id);
        bool _parse_alloc(std::string def_id);
        bool _parse_load(std::string def_id);
        bool _parse_call(std::string def_id);
        bool _parse_cmp(std::string def_id);

        bool _parse_ty(TypeID& ty);
        bool _parse_opnd(Operand **op, TypeID ty, bool is_def);

        Token _bump();
    };
};
