#pragma once

#include <vector>

#include <ucb/core/isel/pat.hpp>

namespace ucb
{
    class Target
    {
    public:
        virtual ~Target() = default;

        virtual std::vector<Pat> load_pats() = 0;
        virtual void dump_bblock(BasicBlock& bblock, std::ostream& out) = 0;
    };
}
