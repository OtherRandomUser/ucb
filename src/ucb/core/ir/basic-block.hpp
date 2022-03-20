#pragma once

#include <ucb/core/ir/dilist.hpp>
#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    class BasicBlock : public DIList<Instruction>
    {
    public:
        BasicBlock(Procedure *parent, std::string id):
            _parent{parent},
            _id(std::move(id))
        {
            assert(_parent);
        }

        const std::string& id() const { return _id; }

        Instruction& append_instr(InstrOpcode op, TypeID ty)
        {
            auto instr = new Instruction(this, op, ty);
            append(instr);
            return *instr;
        }

    private:
        Procedure *_parent;
        std::string _id;
    };
}
