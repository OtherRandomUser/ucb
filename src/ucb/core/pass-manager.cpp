#include <ucb/core/pass-manager.hpp>

namespace  ucb
{
    void PassManager::apply(std::shared_ptr<CompileUnit> unit)
    {
        for (auto& pass: _passes)
        {
            for(auto& proc: unit->procs())
            {
                pass->apply(proc);
            }
        }

        _target->compile(unit);
        //unit->dump(std::cout);
    }
}
