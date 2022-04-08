#include <ucb/core/isel/dp-isel.hpp>

#include <ucb/core/isel/dag.hpp>

namespace ucb
{
    void DynamicISel::run_on_procedure(std::shared_ptr<Procedure> proc)
    {
        for (auto& bblock: proc->bblocks())
        {
            run_on_bblock(bblock);
        }
    }

    void DynamicISel::run_on_bblock(BasicBlock& bblock)
    {
        // compute live ins & live outs

        // build dag
        Dag dag;
        int order = 0;

        for (auto& inst: bblock.insts())
        {
            auto dk = DagDefKind::DDK_REG;

            if (inst.is_terminator())
            {
                dk = DagDefKind::DDK_EXIT;
            }

            if (inst.is_store())
            {
                dk = DagDefKind::DDK_MEM;
            }

            auto n = std::make_shared<DagNode>(order++, inst.op(), dk, inst.ty(), inst.id());

            for (auto& op: inst.opnds())
            {
                if (op.is_def())
                {
                    continue;
                }

                std::shared_ptr<DagNode> arg;

                switch (op.kind())
                {
                default:
                    assert(false && "unreachable");

                case OperandKind::OK_VIRTUAL_REG:
                    arg = dag.get_register(op.get_virtual_reg(), op.ty());
                    n->add_arg(arg);
                    break;

                case OperandKind::OK_INTEGER_CONST:
                    arg = dag.get_imm(op.get_integer_val(), op.ty());
                    n->add_arg(arg);
                    break;

                case OperandKind::OK_UNSIGNED_CONST:
                    arg = dag.get_imm(op.get_unsigned_val(), op.ty());
                    n->add_arg(arg);
                    break;

                case OperandKind::OK_FLOAT_CONST:
                    arg = dag.get_imm(op.get_float_val(), op.ty());
                    n->add_arg(arg);
                    break;

                case OperandKind::OK_BASIC_BLOCK:;
                    // no real need for an operand here
                }
            }

            if (inst.is_side_effect())
            {
                dag.add_root_def(n);
            }
            else
            {
                dag.add_def(n);
            }
        }

        // match
        _pats = _target->load_pats();

        for (auto n: dag.root_nodes())
        {
            recursive_match(n);
        }

        // select
        for (auto n: dag.root_nodes())
        {
            recursive_fill(n, bblock);
        }
    }

    void DynamicISel::recursive_match(std::shared_ptr<DagNode> n)
    {
        if (n->is_leaf())
        {
            return;
        }

        for (auto arg: n->args())
        {
            recursive_match(n);
        }

        Pat *selected = nullptr;
        std::vector<std::shared_ptr<DagNode>> selected_opnds;
        float cost = 9000;

        for (auto& pat: _pats)
        {
            auto res = pat.match(n);

            if (res.is_match)
            {
                float match_cost = pat.cost;

                for (auto opnd: res.selected_opnds)
                {
                    match_cost += opnd->cost();
                }

                if (match_cost < cost)
                {
                    cost = match_cost;
                    selected = &pat;
                    selected_opnds = std::move(res.selected_opnds);
                }
            }
        }

        if (selected == nullptr)
        {
            std::cerr << "failed to match node\n";
            n->dump(std::cerr, nullptr);
            abort();
        }

        n->add_selected_insts(selected->replace(n));
        n->add_selected_args(std::move(selected_opnds));
    }

    void DynamicISel::recursive_fill(std::shared_ptr<DagNode> n, BasicBlock& bblock)
    {
        if (n->is_leaf())
        {
            return;
        }

        for (auto arg: n->selected_args())
        {
            recursive_fill(arg, bblock);
        }

        bblock.append_machine_insts(n->selected_insts());
    }
}
