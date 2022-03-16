#pragma once

#include <iterator>
#include <string>

#include <ucb/core/ir/dilist.hpp>
#include <ucb/core/ir/ilist.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
    class BasicBlock;
    class Procedure;

    enum OperandKind : int
    {
        OK_POISON = 0,
        OK_VIRTUAL_REG,
        // OT_GLOBAL_VARIABLE,
        // OT_STRING_LITERAL,
        OK_INTEGER_CONST,
        OK_UNSIGNED_CONST,
        OK_FLOAT_CONST,
        OK_BASIC_BLOCK
    };

    class Operand : public IListNode<Operand>
    {
    public:
        Operand(Procedure *parent, VirtualRegister *reg, bool is_def);
        Operand(Procedure *parent, BasicBlock *bblock);
        Operand(Procedure *parent, long int val, TypeID ty);
        Operand(Procedure *parent, unsigned long val, TypeID ty);
        Operand(Procedure *parent, double val, TypeID ty);

        OperandKind kind() const { return _kind; }
        const TypeID ty() const { return _ty; }

        VirtualRegister* get_virtual_reg();
        const VirtualRegister* get_virtual_reg() const;
        BasicBlock* get_bblock();
        const BasicBlock* get_bblock() const;
        long int get_integer_val() const;
        double get_float_val() const;

    private:
        Operand(Procedure *parent);

        Procedure *_parent;
        TypeID _ty;
        OperandKind _kind;

        bool _is_def;

        VirtualRegister *_reg;
        BasicBlock *_bblock;
        long int _integer_val;
        unsigned long _unsigned_val;
        double _float_val;
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
