#pragma once

#include <ucb/core/target.hpp>
#include <ucb/core/regalloc/interference-graph.hpp>
#include <ucb/core/regalloc/regalloc.hpp>

namespace  ucb
{
    class GraphColoringRegAlloc : public RegAlloc
    {
    public:
        GraphColoringRegAlloc(std::shared_ptr<Target> target):
            _target(std::move(target))
        {
        }

        void run_on_procedure(std::shared_ptr<Procedure> proc, bool debug) override;

    private:
        std::shared_ptr<Target> _target;

        void select_registers(Procedure& proc, InterferenceGraph& ig);
    };
}
