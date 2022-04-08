#include <ucb/core/isel/pat.hpp>

namespace ucb
{
    MatchResult Pat::match(std::shared_ptr<DagNode> n)
    {
        assert(n);
        return pat.match(n);
    }

    MatchResult PatNode::match(std::shared_ptr<DagNode> n)
    {
        assert(n);
        MatchResult res;
        res.is_match = false;

        // TODO relax to is supper of so that pointers work
        if (ty != n->ty())
        {
            return res;
        }

        if (kind == PatNode::Inst)
        {
            if (opc != n->opc())
            {
                return res;
            }

            auto& args = n->args();

            if (opnds.size() != args.size())
            {
                return res;
            }

            auto ita = opnds.begin();
            auto itb = args.begin(); 

            while (ita != opnds.end() && itb != args.end())
            {
                auto match_res = ita->match(*itb);

                if (!match_res.is_match)
                {
                    return res;
                }

                res.selected_opnds.insert(
                    res.selected_opnds.end(),
                    std::make_move_iterator(match_res.selected_opnds.begin()),
                    std::make_move_iterator(match_res.selected_opnds.begin())
                );

                ita++;
                itb++;
            }
        }

        if (kind == PatNode::Opnd)
        {
            switch (opnd)
            {
            case OperandKind::OK_POISON:
                assert(false && "unreachable");

            case OperandKind::OK_VIRTUAL_REG:
                if (n->kind() != DagDefKind::DDK_REG) { return res; }
                break;

            case OperandKind::OK_INTEGER_CONST:
            case OperandKind::OK_UNSIGNED_CONST:
            case OperandKind::OK_FLOAT_CONST:
                if (n->kind() != DagDefKind::DDK_IMM) { return res; }
                break;

            case OperandKind::OK_BASIC_BLOCK:
                if (n->kind() != DagDefKind::DDK_ADDR) { return res; }
                break;
            }

            res.selected_opnds.push_back(std::move(n));
        }

        res.is_match = true;

        return res;
    }

    void PatNode::get_args(std::shared_ptr<DagNode> n, std::vector<std::shared_ptr<DagNode>>& args)
    {
        assert(n);

        if (kind == PatNode::Inst)
        {
            auto& args_ = n->args();

            assert(opnds.size() == args_.size());

            auto ita = opnds.begin();
            auto itb = args_.begin(); 

            while (ita != opnds.end() && itb != args_.end())
            {
                ita->get_args(*itb, args);

                ita++;
                itb++;
            }
        }

        if (kind == PatNode::Opnd)
        {
            args.push_back(std::move(n));
        }
    }

    std::list<MachineInstruction> Pat::replace(std::shared_ptr<DagNode> n)
    {
        assert(n);
        std::list<MachineInstruction> res;

        std::vector<std::shared_ptr<DagNode>> args;
        pat.get_args(n, args);

        for (auto& r: reps)
        {
            MachineInstruction inst;

            inst.opc = r.opc;
            inst.id = n->id();

            for (auto op: r.opnds)
            {
                MachineOperand mop;
                auto on = args[op];

                switch (on->kind())
                {
                case DagDefKind::DDK_NONE:
                    assert(false && "unreachable");

                case DagDefKind::DDK_REG:
                    mop.kind = MachineOperand::Register;
                    mop.val = std::bit_cast<std::uint64_t>(on->reg());
                    break;

                case DagDefKind::DDK_MEM:
                    assert(false && "unreachable");
                    // TODO

                case DagDefKind::DDK_IMM:
                    mop.kind = MachineOperand::Imm;
                    mop.val = on->imm_val();
                    break;

                case DagDefKind::DDK_ADDR:
                    assert(false && "unreachable");
                    // TODO

                case DagDefKind::DDK_ENTRY:
                case DagDefKind::DDK_EXIT:
                    assert(false && "unreachable");
                }

                inst.opnds.push_back(std::move(mop));
            }

            res.push_back(std::move(inst));
        }

        return res;
    }

    std::list<MachineInstruction> RepNode::replace(std::shared_ptr<DagNode> n, std::vector<std::shared_ptr<DagNode>> args)
    {
        assert(n);
        std::list<MachineInstruction> res;



        return res;
    }
}
