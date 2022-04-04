#pragma once

#include <ostream>

#include <ucb/core/isel/isel.hpp>
#include <ucb/core/ir/compile-unit.hpp>

namespace ucb
{
    enum TargetArch
    {
        ARCH_X64
    };

    class TargetMachine
    {
    public:
        TargetMachine(TargetArch arch, std::unique_ptr<ISel> isel):
            _arch{arch},
            _isel(std::move(isel))
        {
        }

        void compile(std::shared_ptr<CompileUnit> unit);

    private:
        TargetArch _arch;
        std::unique_ptr<ISel> _isel;
    };
}
