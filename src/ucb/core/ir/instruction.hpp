#pragma once

#include <iterator>
#include <ostream>
#include <string>

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

    class Operand
    {
    public:
        Operand() = default;
        Operand(Procedure *parent, RegisterID reg, bool is_def);
        Operand(Procedure *parent, int bblock_idx);
        Operand(Procedure *parent, long int val, TypeID ty);
        Operand(Procedure *parent, unsigned long val, TypeID ty);
        Operand(Procedure *parent, double val, TypeID ty);

        const Procedure* parent() const { return _parent; }
        OperandKind kind() const { return _kind; }
        const TypeID ty() const { return _ty; }

        RegisterID get_virtual_reg() const;
        int get_bblock_idx() const;
        long int get_integer_val() const;
        double get_float_val() const;

        void dump(std::ostream& out);

        bool operator != (const Operand& other) const
        {
            return _parent != other._parent
                || _ty != other._ty
                || _kind != other._kind
                || _reg != other._reg
                || _bblock_idx != other._bblock_idx
                || _integer_val != other._integer_val
                || _unsigned_val != other._unsigned_val
                || _float_val != other._float_val;
        }

        bool operator == (const Operand& other) const
        {
            return !(*this != other);
        }

    private:
        explicit Operand(Procedure *parent);

        Procedure *_parent{nullptr};
        TypeID _ty{TypeID::T_ERROR};
        OperandKind _kind{OperandKind::OK_POISON};

        bool _is_def{false};

        RegisterID _reg{NO_REG};
        int _bblock_idx{-1};
        long int _integer_val{0};
        unsigned long _unsigned_val{0};
        double _float_val{0};
    };

    enum InstrOpcode
    {
        OP_NONE = 0,
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

    class Instruction
    {
    public:
        Instruction(BasicBlock *parent, InstrOpcode op, TypeID ty):
            _parent(parent),
            _op{op},
            _ty{ty}
        {
        }

        Instruction(BasicBlock *parent, InstrOpcode op, TypeID ty, std::string id):
            Instruction(parent, op, ty)
        {
            _id = std::move(id);
        }

        std::string dump();
        void verify();

        BasicBlock* parent() { return _parent; }
        const BasicBlock* parent() const { return _parent; }
        InstrOpcode op() const { return _op; }
        TypeID ty() const { return _ty; }
        const std::string& id() const { return _id; }

        void add_operand(Operand opnd);

        void dump(std::ostream& out);

    private:
        BasicBlock *_parent;
        InstrOpcode _op;
        TypeID _ty;
        std::string _id; // TODO use an operand
        std::vector<Operand> _opnds;

        void _dump_opnds(std::ostream& out);
    };
}
