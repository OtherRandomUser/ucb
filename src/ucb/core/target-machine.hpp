#pragma once

#include <ostream>

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
        TargetMachine(TargetArch arch):
            _arch{arch}
        {
        }

        void compile(std::shared_ptr<CompileUnit> unit);

    private:
        TargetArch _arch;
    };
}
