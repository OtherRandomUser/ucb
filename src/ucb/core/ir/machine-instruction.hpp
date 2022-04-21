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
        bool is_def{false};
    };

    struct MachineInstruction
    {
        std::uint64_t opc;
        std::vector<MachineOperand> opnds;
        std::string id;

        MachineInstruction() = default;

        MachineInstruction(std::uint64_t opc_):
            opc{opc_}
        {
        }

        MachineInstruction(std::uint64_t opc_, std::string id_):
            opc{opc_},
            id(std::move(id_))
        {
        }
    };
}
