#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <ucb/core/ir/type.hpp>

namespace ucb
{
    class Procedure;

    struct MachineOperand
    {
        enum Kind
        {
            Imm,
            Register,
            FrameSlot,
            BBlockAddress
        } kind;

        TypeID ty;
        std::uint64_t val;
    };

    struct MachineInstruction
    {
        std::uint64_t opc;
        std::vector<MachineOperand> opnds;
        std::string id;
    };
}
