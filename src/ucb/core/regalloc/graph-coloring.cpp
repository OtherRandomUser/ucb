#include <ucb/core/regalloc/graph-coloring.hpp>

#include <iostream>

#include <ucb/core/regalloc/interference-graph.hpp>

namespace  ucb
{
    void GraphColoringRegAlloc::run_on_procedure(std::shared_ptr<Procedure> proc, bool debug)
    {
        auto reg_classes = _target->load_reg_classes();

        if (debug)
        {
            std::cout << "register allocation for procedure: " << proc->id() << "\n\n";
        }

        for (auto& reg_class: reg_classes)
        {
            // build
            auto ig = build_interference_graph(*proc, reg_class.tys, _target);

            if (debug)
            {
                std::cout << "interference graph:\n";
                ig.dump();
            }

            // select
            // coalesce
            // freeze
            // select
            // spill
        }
    }
}
