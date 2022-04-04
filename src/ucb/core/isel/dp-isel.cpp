#include <ucb/core/isel/dp-isel.hpp>

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
        std::cout << "test" << bblock.id() << "\n";
    }
}
