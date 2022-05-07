#pragma once

#include <ostream>

#include <ucb/core/target.hpp>
#include <ucb/core/ir/compile-unit.hpp>
#include <ucb/core/isel/isel.hpp>
#include <ucb/core/regalloc/regalloc.hpp>

namespace ucb
{
    enum TargetArch
    {
        ARCH_X64
    };

    class TargetMachine
    {
    public:
        TargetMachine(TargetArch arch, std::unique_ptr<ISel> isel, std::unique_ptr<RegAlloc> regalloc, std::shared_ptr<Target> target):
            _arch{arch},
            _isel(std::move(isel)),
            _regalloc(std::move(regalloc)),
            _target(std::move(target))
        {
        }

        void compile(std::shared_ptr<CompileUnit> unit, const std::string& src_file, std::ostream& output);

    private:
        TargetArch _arch;
        std::unique_ptr<ISel> _isel;
        std::unique_ptr<RegAlloc> _regalloc;
        std::shared_ptr<Target> _target;
    };
}
