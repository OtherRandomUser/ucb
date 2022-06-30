#include <ucb/core/target-machine.hpp>

namespace ucb
{
    void TargetMachine::compile(std::shared_ptr<CompileUnit> unit, const std::string& src_file, std::ostream& output)
    {
        for(auto& proc: unit->procs())
        {
            proc->dump(std::cout);

            _isel->run_on_procedure(proc, true);
            _regalloc->run_on_procedure(proc, true);
            _target->stack_lower(*proc);
            //_target->dump_proc(*proc, std::cout);
        }

        _target->print_asm(*unit, output, src_file);
    }
}
