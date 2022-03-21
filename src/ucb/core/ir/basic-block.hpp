#pragma once

#include <ostream>

#include <ucb/core/ir/dilist.hpp>
#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    class BasicBlock :
        public DIList<Instruction>,
        public IListNode<BasicBlock>
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

        Instruction& append_instr(InstrOpcode op, TypeID ty, std::string id)
        {
            auto instr = new Instruction(this, op, ty, std::move(id));
            append(instr);
            return *instr;
        }

        void dump(std::ostream& out) const;
        void dump_ty(std::ostream& out, TypeID ty) const;

    private:
        Procedure *_parent;
        std::string _id;
    };
}
