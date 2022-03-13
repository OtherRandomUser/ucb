#pragma once

#include <iterator>
#include <string>

#include <ucb/core/ir/dilist.hpp>
#include <ucb/core/ir/ilist.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
    class Operand;
    class OperandIterator;
    class Instruction;
    class InstructionIterator;
    class BasicBlock;

    enum OperandTy : int
    {
        OT_SENTINEL,
        OT_VIRTUAL_REG,
        // OT_GLOBAL_VARIABLE,
        // OT_STRING_LITERAL,
        OT_INTEGER_CONST,
        OT_FLOAT_CONST,
        OT_BASIC_BLOCK
    };

    class Operand : public IListNode<Operand>
    {
    public:
        friend OperandIterator;
        friend Instruction;

        static Operand* MakeSentinel();
        static Operand* MakeVirtualReg(VirtualRegister *reg, bool is_def);
        static Operand* MakeBasicBlock(BasicBlock *bblock);
        static Operand* MakeIntegerConst(long int val, Type *ty);
        static Operand* MakeFloatConst(double val, Type *ty);

        bool is_sentinel() const { return _ty == OT_SENTINEL; }
        bool is_value() const { return _ty == OT_VIRTUAL_REG || _ty == OT_INTEGER_CONST || _ty == OT_FLOAT_CONST; }
        bool is_const() const { return _ty == OT_INTEGER_CONST || _ty == OT_FLOAT_CONST; }

        OperandTy ty() const { return _ty; }
        const Type* vt() const { return _vt; }

        VirtualRegister* get_virtual_reg();
        const VirtualRegister* get_virtual_reg() const;
        BasicBlock* get_bblock();
        const BasicBlock* get_bblock() const;
        long int get_integer_val() const;
        double get_float_val() const;


    private:
        Operand(OperandTy ty, const Type *vt, bool is_def):
            _ty{ty},
            _vt{vt},
            _is_def{is_def},
            _next{nullptr}
        {
        }

        OperandTy _ty;
        const Type *_vt;
        bool _is_def;
        Operand *_next;

        union
        {
            VirtualRegister *_virtual_reg;
            BasicBlock *_bblock;
            long int _integer_val;
            double _float_val;
        };
    };

    class OperandIterator
    {
        Operand *_cur;

        using iterator_category = std::forward_iterator_tag;
        using value_type = Operand;
        using difference_type = size_t;
        using pointer = Operand*;
        using reference = Operand&;

        OperandIterator():
            _cur{nullptr}
        {
        }

        OperandIterator(Operand *cur):
            _cur{cur}
        {
        }

        reference operator* () { return *_cur; }

        bool operator != (const OperandIterator& other)
        {
            return _cur != other._cur;
        }

        OperandIterator& operator++ ()
        {
            _cur = _cur->_next;
            return *this;
        }

        OperandIterator operator++ (int)
        {
            auto cur = _cur;
            _cur = _cur->_next;
            return OperandIterator(cur);
        }
    };

    enum InstrOpcode
    {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_REM,
        OP_NOT,
        OP_AND,
        OP_OR,
        OP_XOR,
        OP_SHL,
        OP_SHR,
        OP_CAST,
        OP_CMP,
        OP_BR,
        OP_BRC,
        OP_ALLOC,
        OP_LOAD,
        OP_STORE,
        OP_CP,
        OP_CALL,
        OP_RET
    };

    class Instruction :
        public IList<Operand>,
        public DIListNode<Instruction>
    {
    public:
        Instruction(BasicBlock *parent, InstrOpcode op):
            _parent(parent),
            _op(op)
        {
        }

        std::string dump() const;
        void verify() const;

        BasicBlock* parent() { return _parent; }
        const BasicBlock* parent() const { return _parent; }
        InstrOpcode op() const { return _op; }
        Type* ty() { return _ty; }
        const Type* ty() const { return _ty; }

    private:
        BasicBlock *_parent;
        InstrOpcode _op;
        Type * _ty;
    };
}
