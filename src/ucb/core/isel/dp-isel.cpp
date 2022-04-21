#include <ucb/core/isel/dp-isel.hpp>

#include <ucb/core/isel/dag.hpp>

namespace ucb
{
    void DynamicISel::run_on_procedure(std::shared_ptr<Procedure> proc, bool debug)
    {
        if (debug)
        {
            std::cout << "instruction selection for procedure: " << proc->id() << "\n\n";
        }

        proc->compute_predecessors();

        for (auto& bblock: proc->bblocks())
        {
            run_on_bblock(bblock, debug);
        }

        _target->abi_lower(*proc);
        proc->compute_machine_lifetimes();

        if (debug)
        {
            std::cout << "proc \"" << proc->id() << "\" after instruction selection:\n\n";

            for (auto& bblock: proc->bblocks())
            {
                _target->dump_bblock(bblock, std::cout);
            }
        }
    }

    void DynamicISel::run_on_bblock(BasicBlock& bblock, bool debug)
    {
        if (debug)
        {
            std::cout << "instruction selection for basic block:\n\n";
            bblock.dump(std::cout);
            std::cout << std::endl;
        }

        // build dag
        Dag dag;
        int order = 0;

        std::vector<RegisterID> reg_slots;
        std::uint64_t mem_id = 0;

        for (auto& [reg, vreg]: bblock.parent()->frame())
        {
            auto n = std::make_shared<DagNode>(0, InstrOpcode::OP_NONE, DagDefKind::DDK_MEM, vreg.ty(), "");
            n->reg() = reg;
            n->mem_id() = mem_id++;
            reg_slots.push_back(reg);
            dag.add_def(n);
        }

        for(auto [id, ty]: bblock.live_ins())
        {
            if (std::find(reg_slots.begin(), reg_slots.end(), id) == reg_slots.end())
            {
                auto n = std::make_shared<DagNode>(0, InstrOpcode::OP_NONE, DagDefKind::DDK_REG, ty, "");
                n->reg() = id;
                dag.add_def(n);
            }
        }

        for (auto& inst: bblock.insts())
        {
            auto dk = DagDefKind::DDK_REG;

            if (inst.is_terminator())
            {
                dk = DagDefKind::DDK_EXIT;
            }

            if (inst.is_store())
            {
                dk = DagDefKind::DDK_NONE;
            }

            auto n = std::make_shared<DagNode>(order++, inst.op(), dk, inst.ty(), inst.id());

            for (auto& op: inst.opnds())
            {
                if (op.is_def())
                {
                    n->reg() = op.get_virtual_reg();
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

            if (inst.is_side_effect()
                || bblock.reg_is_live_out(n->reg()))
            {
                dag.add_root_def(n);
            }
            else
            {
                dag.add_def(n);
            }
        }

        if (debug)
        {
            dag.dump(std::cout, *bblock.context());
        }

        std::cout << "built dag" << std::endl;

        // match
        _pats = _target->load_pats();

        for (auto n: dag.root_nodes())
        {
            recursive_match(n, *bblock.context());
        }

        // select
        for (auto n: dag.root_nodes())
        {
            recursive_fill(n, bblock);
        }

        if (debug)
        {
            std::cout << "selected block:" << std::endl;
            _target->dump_bblock(bblock, std::cout);
        }
    }

    void DynamicISel::recursive_match(std::shared_ptr<DagNode> n, CompileUnit& context)
    {
        std::cout << "recursive match call" << std::endl;

        if (n->is_leaf())
        {
            std::cout << "leaf node" << std::endl;
            return;
        }

        for (auto arg: n->args())
        {
            recursive_match(arg, context);
        }

        Pat *selected = nullptr;
        std::vector<std::shared_ptr<DagNode>> selected_opnds;
        float cost = 9000;

        auto count = 0;

        for (auto& pat: _pats)
        {
            std::cout << "matching #" << ++count << std::endl;

            auto res = pat.match(n);

            if (res.is_match)
            {
                float match_cost = pat.cost;

                for (auto opnd: res.selected_opnds)
                {
                    std::cout << "opnd cost: " << opnd->cost() << std::endl;
                    match_cost += opnd->cost();
                }

                std::cout << "opnd count: " << res.selected_opnds.size() << std::endl;
                std::cout << "match cost: " << match_cost << std::endl;

                if (match_cost < cost)
                {
                    std::cout << "updated match to #" << count << std::endl;

                    cost = match_cost;
                    selected = &pat;
                    selected_opnds = std::move(res.selected_opnds);
                }
            }
        }

        if (selected == nullptr)
        {
            std::cerr << "failed to match node\n";
            n->dump(std::cerr, context);
            abort();
        }

        n->cost() = cost;
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
