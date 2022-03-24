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
        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto id = _cur;
        CHECK_TK(id, id.ty == TokenType::ID_GLOBAL, "global identifier");

        auto next = _bump();

        if (next.ty == '(')
        {
            return _parse_func(ty, std::string(id.lexema));
        }
        else if (next.ty == '=')
        {
            return _parse_global(); // TODO
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

    bool Parser::_parse_func(TypeID ret_ty, std::string id)
    {
        auto lp = _cur;
        CHECK_TK(lp, lp.ty == '(', "'('");

        _bump();
        ProcSignature sig(ret_ty);
        if (!_parse_param_defs(ret_ty, sig))
        {
            return false;
        }

        _proc = _compile_unit->add_procedure(sig, id);
        auto idx = _proc->add_bblock("entry");
        _bblock = _proc->get_bblock(idx);

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

        _proc = nullptr;
        _bblock = nullptr;
        _bump();
        return true;
    }

    bool Parser::_parse_param_defs(TypeID ret_ty, ProcSignature& sig)
    {
        while (true)
        {
            TypeID ty;
            if (!_parse_ty(ty))
            {
                return false;
            }

            auto id = _cur;
            CHECK_TK(id, id.ty == TokenType::ID_LOCAL, "local identifier");

            sig.args().emplace_back(id.lexema, ty);

            auto c = _bump();

            if (c.ty == ')')
            {
                break;
            }

            CHECK_TK(c, c.ty == ',', "')' or ','");
            _bump();
        }

        _bump();
        return true;
    }

    bool Parser::_parse_statement()
    {
        switch (_cur.ty)
        {
        case TokenType::ID_OTHER:
            return _parse_label();

        case TokenType::ID_LOCAL:
        // case TokenType::ID_GLOBAL: no globals
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

        auto idx = _proc->find_bblock(std::string(id.lexema));

        if (idx == -1)
        {
            idx = _proc->add_bblock(std::string(id.lexema));
        }

        _bblock = _proc->get_bblock(idx);

        if (_bblock == nullptr)
        {
            ERROR("failed to create basic block \"{}\"\n", id, id.lexema);
        }

        _bump();
        return true;
    }

    bool Parser::_parse_assign_statement()
    {
        auto def_id = _cur;
        CHECK_TK(def_id, def_id.ty == TokenType::ID_LOCAL, "local identifier");

        auto as = _bump();
        CHECK_TK(as, as.ty == '=', "'='");

        auto op = _bump();

        if (tk_is_bin_op(op))
        {
            return _parse_bin_op(std::string(def_id.lexema));
        }
        else if (op.ty == TokenType::OP_NOT || op.ty == TokenType::OP_CP)
        {
            return _parse_unnary_op(std::string(def_id.lexema));
        }
        else if (op.ty == TokenType::OP_CAST)
        {
            return _parse_cast(std::string(def_id.lexema));
        }
        else if (op.ty == TokenType::OP_ALLOC)
        {
            return _parse_alloc(std::string(def_id.lexema));
        }
        else if (op.ty == TokenType::OP_LOAD)
        {
            return _parse_load(std::string(def_id.lexema));
        }
        else if (op.ty == TokenType::OP_CALL)
        {
            return _parse_call(std::string(def_id.lexema));
        }
        else if (op.ty == TokenType::OP_CMP)
        {
            return _parse_cmp(std::string(def_id.lexema));
        }

        ERROR("expected an asignment statement but found '{}'", op, op.lexema);
        return false;
    }

    bool Parser::_parse_store()
    {
        auto op = _cur;
        CHECK_TK(op, op.ty == TokenType::OP_STORE, "'store'");
        _bump();

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        Operand val;
        if (!_parse_opnd(val, ty, false))
        {
            return false;
        }

        TypeID ty_ref;
        if (!_parse_ty(ty_ref))
        {
            return false;
        }

        Operand ref;
        if (!_parse_opnd(ref, ty_ref, false))
        {
            return false;
        }

        auto& inst = _bblock->append_instr(InstrOpcode::OP_STORE, ty);
        inst.add_operand(val);
        inst.add_operand(ref);

        return true;
    }

    bool Parser::_parse_br()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_BR, "br");
        _bump();

        Operand tgt;
        if (!_parse_opnd(tgt, TypeID::T_STATIC_ADDRESS, false))
        {
            return false;
        }

        auto& inst = _bblock->append_instr(InstrOpcode::OP_BR, TypeID::T_STATIC_ADDRESS);
        inst.add_operand(tgt);

        return true;
    }

    bool Parser::_parse_brc()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_BRC, "brc");

        _bump();
        CHECK_TK(_cur, _cur.ty == TokenType::TY_BOOL, "bool");

        _bump();

        Operand cnd;
        if (!_parse_opnd(cnd, TypeID::T_BOOL, true))
        {
            return false;
        }

        Operand lt;
        if (!_parse_opnd(lt, TypeID::T_STATIC_ADDRESS, false))
        {
            return false;
        }

        Operand lf;
        if (!_parse_opnd(lf, TypeID::T_STATIC_ADDRESS, false))
        {
            return false;
        }

        auto& inst = _bblock->append_instr(InstrOpcode::OP_BRC, TypeID::T_STATIC_ADDRESS);
        inst.add_operand(cnd);
        inst.add_operand(lt);
        inst.add_operand(lf);

        return true;
    }

    bool Parser::_parse_ret()
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_RET, "ret");

        _bump();
        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto& inst = _bblock->append_instr(InstrOpcode::OP_RET, ty);

        if (ty != TypeID::T_VOID)
        {
            Operand opnd;
            if (!_parse_opnd(opnd, ty, false))
            {
                return false;
            }

            inst.add_operand(opnd);
        }

        return true;
    }

    bool Parser::_parse_bin_op(std::string def_id)
    {
        auto optk = _cur;
        CHECK_TK(optk, tk_is_bin_op(optk), "a binary operation");

        // accept op
        _bump();

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }
        
        auto reg = _proc->find_vreg(def_id);

        if (reg == NO_REG)
        {
            _proc->add_vreg(def_id, ty);
        }

        auto def = _proc->operand_from_vreg(def_id, true);

        Operand lhs;
        if (!_parse_opnd(lhs, ty, false))
        {
            return false;
        }

        Operand rhs;
        if (!_parse_opnd(rhs, ty, false))
        {
            return false;
        }

        auto op = InstrOpcode::OP_ADD;

        switch (optk.ty)
        {
            case TokenType::OP_ADD:
                op = InstrOpcode::OP_ADD;
                break;

            case TokenType::OP_SUB:
                op = InstrOpcode::OP_SUB;
                break;

            case TokenType::OP_MUL:
                op = InstrOpcode::OP_MUL;
                break;

            case TokenType::OP_DIV:
                op = InstrOpcode::OP_DIV;
                break;

            case TokenType::OP_REM:
                op = InstrOpcode::OP_REM;
                break;

            case TokenType::OP_AND:
                op = InstrOpcode::OP_AND;
                break;

            case TokenType::OP_OR:
                op = InstrOpcode::OP_OR;
                break;

            case TokenType::OP_XOR:
                op = InstrOpcode::OP_XOR;
                break;

            case TokenType::OP_SHL:
                op = InstrOpcode::OP_SHL;
                break;

            case TokenType::OP_SHR:
                op = InstrOpcode::OP_SHR;
                break;

            default:
                std::cerr << "Unknown op\n";
                abort();
        }

        auto& inst = _bblock->append_instr(op, ty);
        inst.add_operand(def);
        inst.add_operand(lhs);
        inst.add_operand(rhs);

        return true;
    }

    bool Parser::_parse_unnary_op(std::string def_id)
    {
        auto op = _cur;
        CHECK_TK(op,op.ty == TokenType::OP_NOT || op.ty == TokenType::OP_CP, "an unnary op");

        _bump();

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto reg = _proc->find_vreg(def_id);

        if (reg == NO_REG)
        {
            _proc->add_vreg(def_id, ty);
        }

        auto def = _proc->operand_from_vreg(def_id, true);

        Operand opnd;
        if (!_parse_opnd(opnd, ty, false))
        {
            return false;
        }

        auto opc = op.ty == TokenType::OP_NOT
            ? InstrOpcode::OP_NOT
            : InstrOpcode::OP_CP;

        auto &inst = _bblock->append_instr(opc, ty);
        inst.add_operand(def);
        inst.add_operand(opnd);

        return true;
    }

    bool Parser::_parse_cast(std::string def_id)
    {
        CHECK_TK(_cur, _cur.ty ==TokenType::OP_CAST, "cast");

        _bump();

        TypeID opnd_ty;
        if (!_parse_ty(opnd_ty))
        {
            return false;
        }

        auto reg = _proc->find_vreg(def_id);

        if (reg == NO_REG)
        {
            _proc->add_vreg(def_id, opnd_ty);
        }

        auto def = _proc->operand_from_vreg(def_id, true);

        Operand opnd;
        if (!_parse_opnd(opnd, opnd_ty, false))
        {
            return false;
        }

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto& inst = _bblock->append_instr(InstrOpcode::OP_CAST, ty);
        inst.add_operand(def);
        inst.add_operand(opnd);

        return true;
    }

    bool Parser::_parse_alloc(std::string def_id)
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_ALLOC, "alloc");

        _bump();

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        _proc->add_frame_slot(def_id, ty); // TODO should be ptr of ty

        return true;
    }

    bool Parser::_parse_load(std::string def_id)
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_LOAD, "load");

        _bump();

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto reg = _proc->find_vreg(def_id);

        if (reg == NO_REG)
        {
            _proc->add_vreg(def_id, ty);
        }

        auto def = _proc->operand_from_vreg(def_id, true);

        TypeID ptr_ty;
        if (!_parse_ty(ptr_ty))
        {
            return false;
        }

        Operand opnd;
        if (!_parse_opnd(opnd, ptr_ty, false))
        {
            return false;
        }

        auto&inst = _bblock->append_instr(InstrOpcode::OP_LOAD, ty);
        inst.add_operand(def);
        inst.add_operand(opnd);

        return true;
    }

    bool Parser::_parse_call(std::string def_id)
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_CALL, "call");

        _bump();

        TypeID ty;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto reg = _proc->find_vreg(def_id);

        if (reg == NO_REG)
        {
            _proc->add_vreg(def_id, ty);
        }

        auto def = _proc->operand_from_vreg(def_id, true);

        auto id = _cur;
        CHECK_TK(id, id.ty == TokenType::ID_GLOBAL, "a global identifier");
        auto lp = _bump();

        auto& inst = _bblock->append_instr(InstrOpcode::OP_CALL, ty, std::string(id.lexema));
        inst.add_operand(def);

        CHECK_TK(lp, lp.ty == '(', "'('");
        _bump();

        while (true)
        {
            auto opnd_ty = TypeID::T_ERROR;
            Operand opnd;
            if (!_parse_opnd(opnd, opnd_ty, false))
            {
                return false;
            }

            inst.add_operand(opnd);

            auto c = _cur;

            if (c.ty == ')')
            {
                break;
            }

            CHECK_TK(c, c.ty == ',', "')' or ','");
        }

        _bump();

        return true;
    }

    bool Parser::_parse_cmp(std::string def_id)
    {
        CHECK_TK(_cur, _cur.ty == TokenType::OP_CMP, "cmp");

        auto mode = _bump();
        std::vector<std::string> modes = {"eq", "ne", "lt", "le", "gt", "ge"};
        CHECK_TK(mode, mode.ty == TokenType::ID_OTHER && std::find(modes.begin(), modes.end(), mode.lexema) != modes.end(), "a comparison mode");

        _bump();

        auto ty = TypeID::T_ERROR;
        if (!_parse_ty(ty))
        {
            return false;
        }

        auto reg = _proc->find_vreg(def_id);

        if (reg == NO_REG)
        {
            _proc->add_vreg(def_id, TypeID::T_BOOL);
        }

        auto def = _proc->operand_from_vreg(def_id, true);

        Operand lhs;
        if (!_parse_opnd(lhs, ty, false))
        {
            return false;
        }

        Operand rhs;
        if (!_parse_opnd(rhs, ty, false))
        {
            return false;
        }

        // TODO the mode should be separated, but i cant be bothered
        auto& inst = _bblock->append_instr(InstrOpcode::OP_CMP, ty, std::string(mode.lexema));
        inst.add_operand(def);
        inst.add_operand(lhs);
        inst.add_operand(rhs);

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

            ty = _compile_unit->get_arr_ty(sub, std::stoi(std::string(sz.lexema)));
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
        auto t = _cur;
        _bump();

        switch(t.ty)
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

    bool Parser::_parse_opnd(Operand& op, TypeID ty, bool is_def)
    {
        op = Operand();

        if (_cur.ty == TokenType::ID_LOCAL)
        {
            if (ty == TypeID::T_STATIC_ADDRESS)
            {
                auto lex = std::string(_cur.lexema);

                if (_proc->find_bblock(lex) == -1)
                {
                    _proc->add_bblock(lex);
                }

                op = _proc->operand_from_bblock(lex);
            }
            else
            {
                op = _proc->operand_from_vreg(std::string(_cur.lexema), is_def);

                if (op == Operand())
                {
                    ERROR("local identifier \"{}\" not found", _cur, _cur.lexema);
                }
            }

            _bump();
            return true;
        }

        if (_cur.ty == TokenType::LT_INT)
        {
            if (ty_is_signed_int(ty))
            {
                auto val = std::stol(std::string(_cur.lexema));
                op = Operand(_proc, val, ty);
            }
            else if (ty_is_unsigned_int(ty))
            {
                auto val = std::stoul(std::string(_cur.lexema));
                op = Operand(_proc, val, ty);
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

            auto val = std::stod(std::string(_cur.lexema));
            op = Operand(_proc, val, ty);

            _bump();
            return true;
        }

        ERROR("expected an operand but found \'{}\'", _cur, _cur.lexema);
    }

    Token Parser::_bump()
    {
        _cur = _lex.bump();

        while (tk_is_transient(_cur) && _cur.ty != TokenType::END_OF_FILE)
        {
            _cur = _lex.bump();
        }

        return _cur;
    }
};
