#pragma once

#include <cassert>
#include <list>
#include <ostream>

#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/machine-instruction.hpp>
#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    class BasicBlock
    {
    public:
        BasicBlock(Procedure *parent, std::string id):
            _parent{parent},
            _id(std::move(id))
        {
            assert(_parent);
        }

        const Procedure* parent() const { return _parent; }
        const std::string& id() const { return _id; }

        Instruction& append_instr(InstrOpcode op, TypeID ty)
        {
            return _insts.emplace_back(this, op, ty);
        }

        Instruction& append_instr(InstrOpcode op, TypeID ty, std::string id)
        {
            return _insts.emplace_back(this, op, ty, std::move(id));
        }

        std::list<Instruction>& insts() { return _insts; }

        void dump(std::ostream& out);
        void dump_ty(std::ostream& out, TypeID ty);

    private:
        Procedure *_parent;
        std::string _id;
        std::list<Instruction> _insts;
    };
}
