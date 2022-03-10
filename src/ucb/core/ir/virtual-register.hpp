#pragma once

#include <string>
#include <vector>

#include <ucb/core/ir/type.hpp>

namespace ucb
{
    class Procedure;
    class Instruction;

    class VirtualRegister
    {
    public:
        friend Procedure;
        friend Instruction;

        Procedure* parent() { return _parent; }
        const Procedure* parent() const { return _parent; }
        std::string_view id() const { return _id; }
        const Type* ty() const { return _ty; }

    private:
        Procedure *_parent;
        std::string _id;
        Type *_ty;

        std::vector<Instruction*> _uses;
        std::vector<Instruction*> _defs;
    };
}
