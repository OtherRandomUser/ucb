#include <ucb/core/ir/instruction.hpp>

#include <cassert>

namespace ucb
{
    static Operand* Operand::MakeSentinel()
    {
        return new Operand(OT_SENTINEL, nullptr, false);
    }

    static Operand* Operand::MakeVirtualReg(VirtualRegister *reg, bool is_def)
    {
        assert(reg && "Virtual register is null");
        auto opnd = new Operand(OT_VIRTUAL_REG, reg->ty(), is_def);
        opnd._virtual_reg = reg;
        return opnd;
    }

    static Operand* Operand::MakeBasicBlock(BasicBlock *bblock)
    {
        assert(reg && "Basic Block is null");
        auto opnd = new Operand(OT_BASIC_BLOCK, nullptr, false);
        opnd._bblock = bblock;
        return opnd;
    }

    static Operand* Operand::MakeIntegerConst(long int val, Type *ty)
    {
        assert(ty && "Type is null");
        assert(ty->as_int_ty()->kind() == IK_FLOAT && "expected an integer type but got float");
        auto opnd = new Operand(OT_INTEGER_CONST, ty, false);
        opnd._integer_val = val; 
        return opnd;
    }

    static Operand* Operand::MakeFloatConst(double val, Type *ty)
    {
        assert(ty && "Type is null");
        assert(ty->as_int_ty()->kind() != IK_FLOAT && "expected a float type but got float");
        auto opnd = new Operand(OT_FLOAT_CONST, ty, false);
        opnd._float_val = val; 
        return opnd;
    }

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

    std::string Instruction::dump() const
    {
        // TODO do this after writing dumps on types vregs and bblocks
        return "instruction";
    }

    void verify() const
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

    Operand& Instruction::add_operand(Operand *opnd)
    {
        assert(opnd);

        if (_head == nullptr)
        {
            _head = opnd;
            _tail = opnd;
            return *opnd;
        }

        _tail->_next = opnd;
        _tail = opnd;
        return *opnd;
    }

    Operand& insert_operand(Operand *opnd, int idx)
    {
        assert(opnd);

        auto it = begin();
        count = 0;

        if (idx == 0)
        {
            opnd->_next = _head;
            _head = opnd;

            if (_tail == nullptr)
            {
                _tail = _head;
            }

            return;
        }

        while (it != end())
        {
            if (count == idx - 1)
            {
                opnd->_next = it->_next;
                it->next = opnd;

                if (it == _tail)
                {
                    _tail = _opnd;
                }

                return;
            }

            it++;
        }

        std::cerr << "operand index out of bounds" << std::endl;
        abort();
    }

    void remove_operand(Operand *opnd)
    {
        assert(opnd);

        auto it = begin();
        int count = 0;

        if (idx == 0 && _head != nullptr)
        {
            auto aux = _head;
            _head = _head->_next;
            delete aux;
            return;
        }

        while (it != end())
        {
            if (count == idx - 1)
            {
                auto aux = it->_next;

                if (aux == nullptr)
                {
                    break;
                }

                it->_next = aux;

                if (aux == _tail)
                {
                    _tail == it;
                }

                delete aux;
            }

            it++;
        }

        std::cerr << "operand index out of bounds" << std::endl;
        abort();
    }

    OperandIterator begin()
    {
        return OperandIterator(_head);
    }

    OperandIterator end()
    {
        return OperandIterator(nullptr);
    }
}
