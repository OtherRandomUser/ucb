#pragma once

#include <vector>

#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/isel/dag.hpp>

namespace ucb
{
    struct PatNode
    {
        enum Kind
        {
            Inst,
            Opnd
        } kind;

        TypeID ty;
        InstrOpcode opc;
        OperandKind opnd;

        std::vector<PatNode> opnds;

        bool match(std::shared_ptr<DagNode> n);
        void get_args(std::shared_ptr<DagNode> n, std::vector<std::shared_ptr<DagNode>>& args);
    };

    struct RepNode
    {
        TypeID ty;
        MachineOpc opc;
        std::vector<std::uint8_t> opnds;

        std::list<MachineIntruction> replace(std::shared_ptr<DagNode> n, std::vector<std::shared_ptr<DagNode>> args);
    };

    struct Pat
    {
        std::uint8_t cost;

        PatNode pat;
        std::vector<RepNode> reps;

        bool match(std::shared_ptr<DagNode> n);
        std::list<MachineIntruction> replace(std::shared_ptr<DagNode> n);
    };
}
