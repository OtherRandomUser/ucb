#pragma once

#include <vector>

#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/isel/dag.hpp>

namespace ucb
{
    struct MatchResult
    {
        bool is_match;
        std::vector<std::shared_ptr<DagNode>> selected_opnds;
    };

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
        std::string id;

        std::vector<PatNode> opnds;

        MatchResult match(std::shared_ptr<DagNode> n, TypeID same_ty);
        void get_args(std::shared_ptr<DagNode> n, std::vector<std::shared_ptr<DagNode>>& args);
    };

    struct RepNode
    {
        TypeID ty;
        MachineOpc opc;
        std::vector<std::int8_t> opnds;

        bool def_is_also_use{false};
    };

    struct Pat
    {
        std::uint8_t cost;

        PatNode pat;
        std::vector<RepNode> reps;

        MatchResult match(std::shared_ptr<DagNode> n);
        std::list<MachineInstruction> replace(std::shared_ptr<DagNode> n);
    };
}
