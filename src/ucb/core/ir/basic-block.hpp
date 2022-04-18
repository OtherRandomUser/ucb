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

        bool compute_live_ins();

        Instruction& append_instr(InstrOpcode op, TypeID ty)
        {
            return _insts.emplace_back(this, op, ty);
        }

        Instruction& append_instr(InstrOpcode op, TypeID ty, std::string id)
        {
            return _insts.emplace_back(this, op, ty, std::move(id));
        }

        void append_machine_insts(std::list<MachineInstruction> insts)
        {
            _machine_insts.insert(
                _machine_insts.end(),
                std::make_move_iterator(insts.begin()),
                std::make_move_iterator(insts.end())
            );
        }

        void clear_dataflow();

        std::list<Instruction>& insts() { return _insts; }
        std::list<MachineInstruction>& machine_insts() { return _machine_insts; }

        std::vector<BasicBlock*>& predecessors() { return _predecessors; }
        std::vector<BasicBlock*>& successors() { return _successors; }
        std::vector<std::pair<RegisterID, TypeID>>& live_ins() { return _live_ins; }

        CompileUnit* context();

        void dump(std::ostream& out);
        void dump_ty(std::ostream& out, TypeID ty);

    private:
        Procedure *_parent;
        std::string _id;
        std::list<Instruction> _insts;
        std::list<MachineInstruction> _machine_insts;

        std::vector<BasicBlock*> _predecessors;
        std::vector<BasicBlock*> _successors;
        std::vector<std::pair<RegisterID, TypeID>> _live_ins;
        std::vector<std::pair<RegisterID, TypeID>> _live_outs;
    };
}
