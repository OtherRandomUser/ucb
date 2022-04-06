#include <ucb/core/ir/instruction.hpp>

#include <cassert>

#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/ir/type.hpp>

namespace ucb
{
    Operand::Operand(Procedure *parent):
        _parent{parent},
        _ty{T_VOID},
        _kind{OperandKind::OK_POISON},
        _is_def{false},
        _reg{NO_REG},
        _bblock_idx{-1},
        _integer_val{0},
        _unsigned_val{0},
        _float_val{0}
    {
        assert(parent && "parent is null");
    }

    Operand::Operand(Procedure *parent, RegisterID reg, bool is_def):
        Operand(parent)
    {
        assert(reg != NO_REG && "Virtual register is null");
        _kind = OperandKind::OK_VIRTUAL_REG;
        auto r = _parent->get_register(reg);
        assert(r && "Virtual register not found");
        _ty = r->ty();
        _is_def = is_def;
        _reg = reg;
    }

    Operand::Operand(Procedure *parent, int bblock_idx):
        Operand(parent)
    {
        assert(bblock_idx != -1 && "Basic Block is null");
        _kind = OperandKind::OK_BASIC_BLOCK;
        _ty = T_STATIC_ADDRESS;
        _bblock_idx = bblock_idx;
    }

    Operand::Operand(Procedure *parent, long int val, TypeID ty):
        Operand(parent)
    {
        assert(ty_is_signed_int(ty) && "expected an integer type");
        _kind = OperandKind::OK_INTEGER_CONST;
        _ty = ty;
        _integer_val = val; 
    }

    Operand::Operand(Procedure *parent, unsigned long val, TypeID ty):
        Operand(parent)
    {
        assert(ty_is_unsigned_int(ty) && "expected an unsigned integer type");
        _kind = OperandKind::OK_UNSIGNED_CONST;
        _ty = ty;
        _unsigned_val = val; 
    }

    Operand::Operand(Procedure *parent, double val, TypeID ty):
        Operand(parent)
    {
        assert(ty_is_float(ty) && "expected a float type");
        _kind = OperandKind::OK_FLOAT_CONST;
        _ty = ty;
        _float_val = val; 
    }

    void Operand::dump(std::ostream& out)
    {
        _parent->dump_ty(out, _ty);
        out << " ";

        switch (_kind)
        {
            case OperandKind::OK_POISON:
                out << "ERROR";
                break;

            case OperandKind::OK_VIRTUAL_REG:
            {
                auto r = _parent->get_register(_reg);
                assert(r);
                out << r->id();
                break;
            }

            case OperandKind::OK_INTEGER_CONST:
                out << _integer_val;
                break;

            case OperandKind::OK_UNSIGNED_CONST:
                out << _unsigned_val;
                break;

            case OperandKind::OK_FLOAT_CONST:
                out << _float_val;
                break;

            case OperandKind::OK_BASIC_BLOCK:
            {
                auto bblock = _parent->get_bblock(_bblock_idx);
                assert(bblock);
                out << "%" << bblock->id();
                break;
            }

            default:
                assert(false && "unreachable");
        }
    }

    void Instruction::add_operand(Operand opnd)
    {
        assert(_parent->parent() == opnd.parent());
        _opnds.push_back(std::move(opnd));
    }

    void Instruction::dump(std::ostream& out)
    {
        out << "\t";

        switch (_op)
        {
            case InstrOpcode::OP_BR:
                out << "br\t";
                break;

            case InstrOpcode::OP_BRC:
                out << "brc\t";
                break;

            case InstrOpcode::OP_STORE:
                out << "store\t";
                break;

            case InstrOpcode::OP_RET:
                out << "ret\t";
                break;

            case InstrOpcode::OP_ADD:
                out << "add\t";
                break;

            case InstrOpcode::OP_SUB:
                out << "sub\t";
                break;

            case InstrOpcode::OP_MUL:
                out << "mul\t";
                break;

            case InstrOpcode::OP_DIV:
                out << "div\t";
                break;

            case InstrOpcode::OP_REM:
                out << "rem\t";
                break;

            case InstrOpcode::OP_NOT:
                out << "not\t";
                break;

            case InstrOpcode::OP_AND:
                out << "and\t";
                break;

            case InstrOpcode::OP_OR:
                out << "or\t";
                break;

            case InstrOpcode::OP_XOR:
                out << "xor\t";
                break;

            case InstrOpcode::OP_SHL:
                out << "shl\t";
                break;

            case InstrOpcode::OP_SHR:
                out << "shr\t";
                break;

            case InstrOpcode::OP_CAST:
                out << "cast\t";
                break;

            case InstrOpcode::OP_CMP:
                out << "cmp " << _id << "\t";
                break;

            case InstrOpcode::OP_LOAD:
                out << "load\t";
                break;

            case InstrOpcode::OP_CP:
                out << "cp\t";
                break;

            case InstrOpcode::OP_CALL:
                out << "call @" << _id << "\t";
                break;

            default:
                assert(false && "unreachable");
        }

        _dump_opnds(out);
        out << '\n';
    }

    void Instruction::_dump_opnds(std::ostream& out)
    {
        auto it = _opnds.begin();
        auto j = "";

        while (it != _opnds.end())
        {
            out << j;
            j = ", ";
            it->dump(out);
            it++;
        }
    }

    void Instruction::verify()
    {
        /* TODO do this after dump is working
        switch(_op)
        {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:

        case OP_REM:
        case OP_NOT:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_SHL:
        case OP_SHR:
        case OP_CAST:
        case OP_CMP:
        case OP_BR:
        case OP_BRC:
        case OP_ALLOC:
        case OP_LOAD:
        case OP_STORE:
        case OP_CP:
        case OP_CALL:
        case OP_RET
        }*/
    }
}
