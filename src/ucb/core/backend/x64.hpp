#pragma once

#include <ucb/core/target.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb::x64
{
    constexpr MachineOpc OPC_NONE = 0;
    constexpr MachineOpc OPC_RET = 1;
    constexpr MachineOpc OPC_MOVE_RR = 2;
    constexpr MachineOpc OPC_MOVE_MR = 3;
    constexpr MachineOpc OPC_MOVE_RM = 4;
    constexpr MachineOpc OPC_ADD_RR = 5;
    constexpr MachineOpc OPC_ADD_RM = 6;
    constexpr MachineOpc OPC_ADD_MR = 7;

    constexpr RegisterID RAX { PREG_START, 64 };
    constexpr RegisterID EAX { RAX.val, 32 };

    constexpr RegisterID RBX { RAX.val + 1, 64 };
    constexpr RegisterID EBX { RBX.val, 32 };

    constexpr RegisterID RCX { RBX.val + 1, 64 };
    constexpr RegisterID ECX { RCX.val, 32 };

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
        std::vector<RegisterClass> load_reg_classes() override;

        bool is_rr_move(MachineOpc opc) override;

        void dump_proc(Procedure& proc, std::ostream& out) override;
        void dump_bblock(BasicBlock& bblock, std::ostream& out) override;
        void abi_lower(Procedure& proc) override;

    private:
        void dump_inst(MachineInstruction& inst, std::ostream& out);
    };
}
