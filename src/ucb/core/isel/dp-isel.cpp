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

            auto n = std::make_shared<DagNode>(order++, inst.op(), dk, inst.ty());

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
        // select
    }
}
