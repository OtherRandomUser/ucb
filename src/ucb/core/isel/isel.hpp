#pragma once

#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    class ISel
    {
    public:
        virtual void run_on_procedure(std::shared_ptr<Procedure> proc, bool debug) = 0;

        virtual ~ISel() = default;
    };
}
