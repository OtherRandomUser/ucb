#pragma once

#include <ucb/core/target-machine.hpp>
#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    class Pass
    {
    public:
        virtual void apply(Procedure& proc) = 0;

        virtual ~Pass() = default;
    };

    class PassManager
    {
    public:
        PassManager(
                std::vector<std::unique_ptr<Pass>> passes,
                std::unique_ptr<TargetMachine> target):
            _passes(std::move(passes)),
            _target(std::move(target))
        {
        }

        void apply(std::shared_ptr<CompileUnit> unit);

    private:
        std::vector<std::unique_ptr<Pass>> _passes;
        std::unique_ptr<TargetMachine> _target;
    };
}
