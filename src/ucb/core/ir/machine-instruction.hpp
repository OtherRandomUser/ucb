#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ucb
{
    struct MachineOperand
    {
        enum Kind
        {
            IImm,
            UImm,
            FImm,
            Register,
            FrameSlot,
            BBlockAddress
        } kind;

        std::uint64_t val;
    };

    struct MachineIntruction
    {
        std::uint64_t opc;
        std::vector<MachineOperand> opnds;
        std::string id;
    };
}
