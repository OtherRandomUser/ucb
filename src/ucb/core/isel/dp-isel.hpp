#pragma once

#include <ucb/core/isel/isel.hpp>
// #include <ucb/core/isel/dag.hpp>
// #include <ucb/core/isel/pat.hpp>
#include <ucb/core/ir/basic-block.hpp>

namespace ucb
{
    class DynamicISel : public ISel
    {
    public:
        void run_on_procedure(std::shared_ptr<Procedure> proc) override;
        void run_on_bblock(BasicBlock& bblock);
    };
}
