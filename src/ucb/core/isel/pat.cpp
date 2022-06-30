#include <ucb/core/isel/pat.hpp>

namespace ucb
{
    bool is_child_of(TypeID t, TypeID super)
    {
        return t.val == super.val && super.size == 0;
    }

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
        auto same_ty = T_NONE;
        return pat.match(n, same_ty);
    }

    MatchResult PatNode::match(std::shared_ptr<DagNode> n, TypeID& same_ty)
    {
        assert(n);
        MatchResult res;
        res.is_match = false;

        TypeID pat_ty = ty;
        TypeID node_ty = n->ty();

        if (ty == T_SAME)
        {
            pat_ty = same_ty;
        }

        if (!match_ty(node_ty, pat_ty))
        {
            return res;
        }

        if (same_ty == T_NONE && (is_child_of(node_ty, T_ANY_U) || is_child_of(node_ty, T_ANY_I) || is_child_of(node_ty, T_ANY_F)))
        {
            same_ty = node_ty;
        }

        if (id != "" && id != n->id())
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

            if (is_va_pat)
            {
                for (auto arg: args)
                {
                    if (arg->kind() == DagDefKind::DDK_IMM
                        || arg->kind() == DagDefKind::DDK_REG)
                    {
                        res.selected_opnds.push_back(std::move(arg));
                    }
                    else
                    {
                        std::cerr << "unsuported def kind on varriadic pattern" << std::endl;
                        abort();
                    }
                }
            }
            else
            {
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
            if (is_va_pat)
            {
                //std::cout << "args size: " << n->args().size() << std::endl;
                for (auto arg: n->args())
                {
                    assert(arg != nullptr);
                    args.push_back(std::move(arg));
                }
            }
            else
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

            auto node2mop = [&](auto& node)
            {
                MachineOperand mop;

                switch (node->kind())
                {
                case DagDefKind::DDK_NONE:
                    assert(false && "unreachable");

                case DagDefKind::DDK_REG:
                    mop.kind = MachineOperand::Register;
                    mop.val = std::bit_cast<std::uint64_t>(node->reg());
                    mop.ty = node->ty();
                    //inst.size = mop.ty.size;
                    break;

                case DagDefKind::DDK_MEM:
                    mop.kind = MachineOperand::FrameSlot;
                    mop.val = node->mem_id();
                    mop.ty = node->ty();
                    break;

                case DagDefKind::DDK_IMM:
                    mop.kind = MachineOperand::Imm;
                    mop.val = node->imm_val();
                    mop.ty = node->ty();
                    break;

                case DagDefKind::DDK_ADDR:
                    mop.kind = MachineOperand::BBlockAddress;
                    mop.val = std::bit_cast<std::uint64_t>(node->bblock_idx());
                    mop.ty = node->ty();
                    break;

                case DagDefKind::DDK_ENTRY:
                case DagDefKind::DDK_EXIT:
                    assert(false && "unreachable");
                }

                return mop;
            };

            if (r.is_va_rep)
            {
                // add the return value if it exists
                auto n_ty = n->ty();

                if (n_ty != T_NONE && n_ty != T_STATIC_ADDRESS && n_ty != T_VOID)
                {
                    MachineOperand mop;
                    mop.kind = MachineOperand::Register;
                    mop.val = std::bit_cast<std::uint64_t>(n->reg());
                    mop.ty = n->ty();
                    mop.is_def = true;
                    mop.is_use = r.def_is_also_use;
                    inst.size = mop.ty.size;
                    inst.opnds.push_back(mop);
                }

                for (auto& arg: args)
                {
                    auto mop = node2mop(arg);

                    if (mop.ty.size > inst.size)
                    {
                        inst.size = mop.ty.size;
                    }

                    inst.opnds.push_back(mop);
                }
            }
            else
            {
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
                        inst.size = mop.ty.size;
                    }
                    else
                    {
                        auto on = args[op];
                        mop = node2mop(on);
                    }

                    if (mop.ty.size > inst.size)
                    {
                        inst.size = mop.ty.size;
                    }

                    inst.opnds.push_back(std::move(mop));
                }
            }

            res.push_back(std::move(inst));
        }

        return res;
    }
}
