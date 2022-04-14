#include <ucb/core/target-machine.hpp>

namespace ucb
{
    void TargetMachine::compile(std::shared_ptr<CompileUnit> unit)
    {
        for(auto& proc: unit->procs())
        {
            _isel->run_on_procedure(proc, true);
        }
    }
}
