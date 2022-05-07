#include <ucb/core/pass-manager.hpp>

namespace  ucb
{
    void PassManager::apply(std::shared_ptr<CompileUnit> unit, const std::string& src_file, std::ostream& output)
    {
        for (auto& pass: _passes)
        {
            for(auto& proc: unit->procs())
            {
                pass->apply(proc);
            }
        }

        _target->compile(unit, std::move(src_file), output);
    }
}
