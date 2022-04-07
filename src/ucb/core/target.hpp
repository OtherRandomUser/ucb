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
    };
}
