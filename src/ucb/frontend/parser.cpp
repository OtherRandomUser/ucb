#include <ucb/frontend/parser.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include <fmt/core.h>

#define DEBUG(T, ...) \
    if (_debug) { fmt::print(stderr, "Parser: " T __VA_OPT__(,) __VA_ARGS__); }

#define DEBUG_LOC(T, TK, ...) \
    if (_debug) { fmt::print(stderr, "Parser: {}:{}:{} " T, _filename, TK.row, TK.col __VA_OPT__(,) __VA_ARGS__); }

#define ERROR(T, TK, ...) \
    { \
        fmt::print(stderr, "\t Parser error at {}:{}:{}: " T, _filename, TK.row, TK.col __VA_OPT__(,) __VA_ARGS__); \
        return false; \
    }

#define CHECK_TK(TK, CONSTRAINT, EXPECTED) \
    if (!(CONSTRAINT)) \
    { \
        ERROR("expected {} but found \'{}\'", TK, EXPECTED, TK.lexema); \
        return false; \
    }


namespace ucb::frontend
{
    bool Parser::parse_unit()
    {
        // load the first token
        _bump();

        while (_cur.ty != TokenType::END_OF_FILE)
        {
            if (!_parse_def())
            {
                return false;
            }
        }

        return true;
    }

    bool Parser::_parse_def()
    {
        if (!_parse_ty())
        {
            return false;
        }

        auto id = _cur;
        CHECK_TK(id, id.ty == TokenType::ID_GLOBAL, "global identifier");

        auto next = _bump();

        if (next.ty == '(')
        {
            return _parse_func();
        }
        else if (next.ty == '=')
        {
            return _parse_global();
        }
        else
        {
            ERROR("expected either a function or global declaration but found '{}'", next, next.lexema);
            return false;
        }
    }

    bool Parser::_parse_global()
    {
        auto eq = _cur;
        CHECK_TK(eq, eq.ty == '=', "'='");

        auto val = _bump();
        CHECK_TK(val, tk_is_literal(val), "a literal value");

        _bump();

        // TODO check types and create the global variable or string

        return true;
    }

    bool Parser::_parse_func()
    {
        auto lp = _cur;
        CHECK_TK(lp, lp.ty == '(', "'('");

        _bump();
        if (!_parse_param_defs())
        {
            return false;
        }

        auto lb = _cur;
        CHECK_TK(lb, lb.ty == '{', "'{'");

        _bump();

        while (_cur.ty != '}')
        {
            CHECK_TK(_cur, _cur.ty != TokenType::END_OF_FILE, "'}'");

            if (!_parse_statement())
            {
                return false;
            }
        }

        _bump();
        return true;
    }

    bool Parser::_parse_param_defs()
    {
        while (true)
        {
            if (!_parse_ty())
            {
                return false;
            }

            auto id = _cur;
            CHECK_TK(id, id.ty == TokenType::ID_LOCAL, "local identifier");

            auto c = _bump();

            if (c.ty == ')')
            {
                break;
            }

            CHECK_TK(c, c.ty == ',', "')' or ','");
        }

        _bump();

        // TODO

        return true;
    }

    bool Parser::_parse_statement()
    {
        switch (_cur.ty)
        {
        case TokenType::ID_OTHER:
            return _parse_label();

        case TokenType::ID_LOCAL:
        case TokenType::ID_GLOBAL:
            return _parse_assign_statement();

        case TokenType::OP_STORE:
            return _parse_store();

        case TokenType::OP_BR:
            return _parse_br();

        case TokenType::OP_BRC:
            return _parse_brc();

        case TokenType::OP_RET:
            return _parse_ret();

        default:
            ERROR("expected a statement but found '{}'", _cur, _cur.lexema);
            return false;
        }
    }

    bool Parser::_parse_label()
    {
        auto id = _cur;
        CHECK_TK(id, id.ty == TokenType::ID_OTHER, "identifier");

        auto p = _bump();
        CHECK_TK(p, p.ty == ':', "':'");

        _bump();

        // TODO create basic block

        return true;
    }

    bool Parser::_parse_assign_statement()
    {
        auto vr = _cur;
        CHECK_TK(vr, vr.ty == TokenType::ID_GLOBAL || vr.ty == TokenType::ID_LOCAL, "identifier");

        auto as = _bump();
        CHECK_TK(as, as.ty == '=', "'='");

        auto op = _bump();

        if (tk_is_bin_op(op))
        {
            return _parse_bin_op();
        }
        else if (op.ty == TokenType::OP_NOT || op.ty == TokenType::OP_CP)
        {
            return _parse_unnary_op();
        }
        else if (op.ty == TokenType::OP_CAST)
        {
            return _parse_cast();
        }
        else if (op.ty == TokenType::OP_ALLOC)
        {
            return _parse_alloc();
        }
        else if (op.ty == TokenType::OP_LOAD)
        {
            return _parse_load();
        }
        else if (op.ty == TokenType::OP_CALL)
        {
            return _parse_call();
        }
        else if (op.ty == TokenType::OP_CMP)
        {
            return _parse_cmp();
        }

        ERROR("expected an asignment statement but found '{}'", op, op.lexema);
        return false;
    }

    bool Parser::_parse_store()
    {
        auto op = _cur;
        CHECK_TK(op, op.ty == TokenType::OP_STORE, "'store'");

        _bump();
        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        _bump();
        if (!_parse_ty())
        {
            return false;
        }

        auto vrb = _cur;
        CHECK_TK(vrb, vrb.ty == TokenType::ID_LOCAL || vrb.ty == TokenType::ID_GLOBAL, "identifier");

        _bump();

        // TODO

        return true;
    }

    bool Parser::_parse_br()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_BR, "br");

        auto target = _bump();
        CHECK_TK(target, target.ty == TokenType::ID_LOCAL, "a local identifier");

        _bump();

        // TODO

        return true;
    }

    bool Parser::_parse_brc()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_BRC, "brc");

        _bump();
        CHECK_TK(_cur, _cur.ty == TokenType::TY_BOOL, "bool");

        if (!_parse_opnd())
        {
            return false;
        }

        auto btrue = _bump();
        CHECK_TK(btrue, btrue.ty == TokenType::ID_LOCAL, "a local identifier");

        auto bfalse = _bump();
        CHECK_TK(bfalse, bfalse.ty == TokenType::ID_LOCAL, "a local identifier");

        _bump();

        // TODO

        return true;
    }

    bool Parser::_parse_ret()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_RET, "ret");

        _bump();
        if (!_parse_ty())
        {
            return false;
        }

        // TODO dont parse a vr if ty is void
        if (!_parse_opnd())
        {
            return false;
        }

        _bump();

        // TODO

        return true;
    }

    bool Parser::_parse_bin_op()
    {
        auto op = _cur;
        CHECK_TK(op, tk_is_bin_op(op), "a binary operation");

        // accept op
        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        // TODO

        return true;
    }

    bool Parser::_parse_unnary_op()
    {
        auto op = _cur;
        CHECK_TK(op,op.ty == TokenType::OP_NOT || op.ty == TokenType::OP_CP, "an unnary op");

        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        // TODO

        return true;
    }

    bool Parser::_parse_cast()
    {
        CHECK_TK(_cur, _cur.ty ==TokenType::OP_CAST, "cast");

        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        if (!_parse_ty())
        {
            return false;
        }

        // TODO

        return true;
    }

    bool Parser::_parse_alloc()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_ALLOC, "alloc");

        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        // TODO

        return true;
    }

    bool Parser::_parse_load()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_LOAD, "load");

        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        // TODO

        return true;
    }

    bool Parser::_parse_call()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_CALL, "call");

        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        auto id = _bump();
        CHECK_TK(id, id.ty == TokenType::ID_GLOBAL, "a global identifier");

        auto lp = _bump();
        CHECK_TK(lp, lp.ty == '(', "'('");

        while (true)
        {
            if (!_parse_opnd())
            {
                return false;
            }

            auto c = _cur;

            if (c.ty == ')')
            {
                break;
            }

            CHECK_TK(c, c.ty == ',', "')' or ','");
        }

        // TODO

        return true;
    }

    bool Parser::_parse_cmp()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_CMP, "cmp");

        auto mode = _bump();
        std::vector<std::string> modes = {"eq", "ne", "lt", "le", "gt", "ge"};
        CHECK_TK(mode, mode.ty == TokenType::ID_OTHER && std::find(modes.begin(), modes.end(), mode.lexema) != modes.end(), "a comparison mode");

        _bump();

        if (!_parse_ty())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        if (!_parse_opnd())
        {
            return false;
        }

        // TODO

        return true;
    }

    bool Parser::_parse_ty(TypeID& ty)
    {
        ty = TypeID::T_ERROR;

        if (_cur.ty == TokenType::ID_OTHER && _cur.lexema == "array")
        {
            auto sz = _bump();
            CHECK_TK(sz, sz.ty == TokenType::LT_INT, "an integer literal");

            auto of = _bump();
            CHECK_TK(of, of.ty == TokenType::ID_OTHER && of.lexema == "of", "of");

            _bump();
            auto sub = TypeID::T_ERROR;
            if (!_parse_ty(sub))
            {
                return false;
            }

            ty = _compile_unit->get_array_ty(sub, std::stoi(sz.lexema));
            return true;
        }

        while (_cur.ty == '^')
        {
            _bump();

            auto sub = TypeID::T_ERROR;
            if (!_parse_ty(sub))
            {
                return false;
            }

            ty = _compile_unit->get_ptr_ty(sub);
            return true;
        }

        CHECK_TK(_cur, tk_is_type(_cur), "a type identifier");
        _bump();

        switch(_cur)
        {
        default:
            return false;

        case TY_VOID:
            ty = TypeID::T_VOID;
            return true;

        case TY_BOOL:
            ty = TypeID::T_BOOL;
            return true;

        case TY_I8:
            ty = TypeID::T_I8;
            return true;

        case TY_I16:
            ty = TypeID::T_I16;
            return true;

        case TY_I32:
            ty = TypeID::T_I32;
            return true;

        case TY_I64:
            ty = TypeID::T_I64;
            return true;

        case TY_U8:
            ty = TypeID::T_U8;
            return true;

        case TY_U16:
            ty = TypeID::T_U16;
            return true;

        case TY_U32:
            ty = TypeID::T_U32;
            return true;

        case TY_U64:
            ty = TypeID::T_U64;
            return true;

        case TY_F32:
            ty = TypeID::T_F32;
            return true;

        case TY_F64:
            ty = TypeID::T_F64;
            return true;
        }
    }

    bool Parser::_parse_opnd(Operand **op, TypeID ty)
    {
        *op = nullptr;

        if (_cur.ty == TokenType::ID_LOCAL)
        {
            *op = _proc->operand_from_id(_cur.lexema);

            if (*op == nullptr)
            {
                ERROR("local identifier \"{}\" not found", _cur, _cur.lexema);
            }

            _bump();
            return true;
        }

        if (_cur.ty == TokenType::LT_INT)
        {
            if (ty_is_signed_int(ty))
            {
                auto val = std::stol(_cur.lexema);
                *op = new Operand(_proc, val, ty);
            }
            else if (ty_is_unsigned_int(ty))
            {
                auto val = std::stoul(_cur.lexema);
                *op = new Operand(_proc, val, ty);
            }
            else
            {
                ERROR("expected an integer type", _cur);
            }

            _bump();
            return true;
        }

        if (_cur.ty == TokenType::LT_FLOAT)
        {
            if (!ty_is_float(ty))
            {
                ERROR("expected a float type", _cur);
            }

            auto val = std::stod(_cur.lexema);
            *op = new Operand(_proc, val, ty);

            _bump();
            return true;
        }

        ERROR("expected an operand but found \'{}\'", _cur, _cur.lexema);
    }

    Token Parser::_bump()
    {
        _cur = _lex.bump();

        while (tk_is_transient(_cur))
        {
            _cur = _lex.bump();
        }

        return _cur;
    }
};
