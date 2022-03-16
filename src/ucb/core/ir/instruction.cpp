#include <ucb/core/ir/instruction.hpp>

#include <cassert>

#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/ir/type.hpp>

namespace ucb
{
    Operand::Operand(Procedure *parent):
        _parent{parent},
        _ty{TypeID::T_VOID},
        _kind{OperandKind::OK_POISON},
        _is_def{false},
        _reg{nullptr},
        _bblock{nullptr},
        _integer_val{0},
        _unsigned_val{0},
        _float_val{0}
    {
        assert(parent && "parent is null");
    }

    Operand::Operand(Procedure *parent, VirtualRegister *reg, bool is_def):
        Operand(parent)
    {
        assert(reg && "Virtual register is null");
        _kind = OperandKind::OK_VIRTUAL_REG;
        _ty = reg->ty();
        _is_def = is_def;
        _reg = reg;
    }

    Operand::Operand(Procedure *parent, BasicBlock *bblock):
        Operand(parent)
    {
        assert(bblock && "Basic Block is null");
        _kind = OperandKind::OK_BASIC_BLOCK;
        _ty = TypeID::T_STATIC_ADDRESS;
        _bblock = bblock;
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
/*
    VirtualRegister* Operand::get_virtual_reg()
    {
        assert(_ty == OT_VIRTUAL_REG && "tried to access operand as virtual register");
        return _virtual_reg;
    }

    const VirtualRegister* Operand::get_virtual_reg() const
    {
        assert(_ty == OT_VIRTUAL_REG && "tried to access operand as virtual register");
        return _virtual_reg;
    }

    BasicBlock* Operand::get_bblock()
    {
        assert(_ty == OT_BASIC_BLOCK && "tried to access operand as basic block");
        return _bblock;
    }

    const BasicBlock* Operand::get_bblock() const
    {
        assert(_ty == OT_BASIC_BLOCK && "tried to access operand as basic block");
        return _bblock;
    }

    long int Operand::get_integer_val() const
    {
        assert(_ty == OT_INTEGER_CONST && "tried to access operand as integer constant");
        return _integer_val;
    }

    double Operand::get_float_val() const
    {
        assert(_ty == OT_FLOAT_CONST && "tried to access operand as float constant");
        return _float_val;
    }
*/
    std::string Instruction::dump() const
    {
        // TODO do this after writing dumps on types vregs and bblocks
        return "instruction";
    }

    void Instruction::verify() const
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
