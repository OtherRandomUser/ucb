#pragma once

#include <ucb/core/target.hpp>
#include <ucb/core/isel/isel.hpp>
// #include <ucb/core/isel/dag.hpp>
// #include <ucb/core/isel/pat.hpp>
#include <ucb/core/ir/basic-block.hpp>

namespace ucb
{
    class DynamicISel : public ISel
    {
    public:
        DynamicISel(std::shared_ptr<Target> target):
            _target(std::move(target))
        {
        }

        void run_on_procedure(std::shared_ptr<Procedure> proc, bool debug) override;
        void run_on_bblock(BasicBlock& bblock, bool debug);

    private:
        std::shared_ptr<Target> _target;
        std::vector<Pat> _pats;

        void recursive_match(std::shared_ptr<DagNode> n, CompileUnit& context);
        void recursive_fill(std::shared_ptr<DagNode> n, BasicBlock& bblock);
    };
}
