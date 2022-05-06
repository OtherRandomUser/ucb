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
            MemAddr,
            FrameSlot,
            BBlockAddress,
            // Offset
        } kind;

        TypeID ty;
        std::uint64_t val;
        std::int32_t offset{0};

        bool is_def{false};
        bool is_use{true};
    };

    struct MachineInstruction
    {
        MachineOpc opc;
        std::int32_t size{0};
        std::vector<MachineOperand> opnds;
        std::string id;

        MachineInstruction() = default;

        MachineInstruction(MachineOpc opc_):
            opc{opc_}
        {
        }

        MachineInstruction(MachineOpc opc_, std::string id_):
            opc{opc_},
            id(std::move(id_))
        {
        }
    };
}
