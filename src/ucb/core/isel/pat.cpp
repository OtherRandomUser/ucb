#include <ucb/core/isel/pat.hpp>

namespace ucb
{
    bool match_ty(TypeID node_ty, TypeID pat_ty)
    {
        if (pat_ty.val == node_ty.val)
        {
            if (pat_ty.size == 0)
            {
                return true;
            }
            else
            {
                return pat_ty.size == node_ty.size;
            }
        }
        else
        {
            return false;
        }
    }

    MatchResult Pat::match(std::shared_ptr<DagNode> n)
    {
        assert(n);
        return pat.match(n, T_NONE);
    }

    MatchResult PatNode::match(std::shared_ptr<DagNode> n, TypeID same_ty)
    {
        assert(n);
        MatchResult res;
        res.is_match = false;

        TypeID pat_ty = ty;

        if (ty == T_SAME)
        {
            pat_ty = same_ty;
        }

        // TODO relax to is supper of so that pointers work
        if (!match_ty(n->ty(), pat_ty))
        {
            return res;
        }

        if (same_ty == T_NONE)
        {
            same_ty = n->ty();
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
                auto match_res = ita->match(*itb, same_ty);

                if (!match_res.is_match)
                {
                    return res;
                }

                res.selected_opnds.insert(
                    res.selected_opnds.end(),
                    std::make_move_iterator(match_res.selected_opnds.begin()),
                    std::make_move_iterator(match_res.selected_opnds.end())
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

            case OperandKind::OK_FRAME_SLOT:
                if (n->kind() != DagDefKind::DDK_MEM) { return res; }
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
            std::cout << "inst rep" << std::endl;
            MachineInstruction inst;

            inst.opc = r.opc;
            inst.id = n->id();

            for (auto op: r.opnds)
            {
                MachineOperand mop;

                if (op == -1)
                {
                    mop.kind = MachineOperand::Register;
                    mop.val = std::bit_cast<std::uint64_t>(n->reg());
                    mop.ty = n->ty();
                    mop.is_def = true;
                    mop.is_use = r.def_is_also_use;
                    std::cout << "!!! minst size " << mop.ty.size << "!!!" << std::endl;
                    inst.size = mop.ty.size;
                }
                else
                {
                    auto on = args[op];

                    switch (on->kind())
                    {
                    case DagDefKind::DDK_NONE:
                        assert(false && "unreachable");

                    case DagDefKind::DDK_REG:
                        mop.kind = MachineOperand::Register;
                        mop.val = std::bit_cast<std::uint64_t>(on->reg());
                        mop.ty = on->ty();
                        std::cout << "!!! minst size " << mop.ty.size << "!!!" << std::endl;
                        inst.size = mop.ty.size;
                        break;

                    case DagDefKind::DDK_MEM:
                        mop.kind = MachineOperand::FrameSlot;
                        mop.val = on->mem_id();
                        mop.ty = on->ty();
                        break;

                    case DagDefKind::DDK_IMM:
                        mop.kind = MachineOperand::Imm;
                        mop.val = on->imm_val();
                        mop.ty = on->ty();
                        break;

                    case DagDefKind::DDK_ADDR:
                        assert(false && "unreachable");
                        // TODO

                    case DagDefKind::DDK_ENTRY:
                    case DagDefKind::DDK_EXIT:
                        assert(false && "unreachable");
                    }
                }

                inst.opnds.push_back(std::move(mop));
            }

            res.push_back(std::move(inst));
        }

        return res;
    }
/*
    std::list<MachineInstruction> RepNode::replace(std::shared_ptr<DagNode> n, std::vector<std::shared_ptr<DagNode>> args)
    {
        assert(n);
        std::list<MachineInstruction> res;



        return res;
    }
*/
}
