#include <ucb/core/ir/compile-unit.hpp>

namespace ucb
{
    std::shared_ptr<Procedure> CompileUnit::add_procedure(ProcSignature sig, std::string id)
    {
        if (get_procedure(id) != nullptr)
        {
            std::cerr << "procedure \"" << id << "already exists\n";
            abort();
        }

        auto proc = std::make_shared<Procedure>(this, std::move(id), std::move(sig));
        _procs.push_back(proc);
        return proc;
    }


    std::shared_ptr<Procedure> CompileUnit::get_procedure(const std::string& id)
    {
        auto it = std::find_if(_procs.begin(), _procs.end(), [&](auto p)
        {
            return p->id() == id;
        });

        if (it != _procs.end())
        {
            return *it;
        }
        else
        {
            return nullptr;
        }
    }
}
