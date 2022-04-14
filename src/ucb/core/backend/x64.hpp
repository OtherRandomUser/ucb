#pragma once

#include <ucb/core/target.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb::x64
{
    constexpr MachineOpc OPC_NONE = 0;
    constexpr MachineOpc OPC_RET = 1;
    constexpr MachineOpc OPC_MOVE = 2;
    constexpr MachineOpc OPC_ADD = 3;

    constexpr RegisterID RAX { PREG_START, 64 };
    constexpr RegisterID RBX { RAX.val + 1, 64 };
    constexpr RegisterID RCX { RBX.val + 1, 64 };

    constexpr RegisterID CALLER_SAVED_REGS[] =
    {
        RAX,
        RBX,
        RCX
    };

    constexpr RegisterID CALLEE_SAVED_REGS[] =
    {
    };

    class X64Target : public Target
    {
    public:
        std::vector<Pat> load_pats() override;

        void dump_bblock(BasicBlock& bblock, std::ostream& out) override;

    private:
        void dump_inst(MachineInstruction& inst, std::ostream& out);
    };
}
