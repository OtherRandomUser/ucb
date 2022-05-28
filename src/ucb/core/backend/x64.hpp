#pragma once

#include <set>

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
    constexpr MachineOpc OPC_ADD = 5;
    constexpr MachineOpc OPC_SUB = 6;
    constexpr MachineOpc OPC_MUL = 7;
    constexpr MachineOpc OPC_IMUL = 8;
    constexpr MachineOpc OPC_DIV = 9;
    constexpr MachineOpc OPC_IDIV = 10;
    constexpr MachineOpc OPC_NOT = 11;
    constexpr MachineOpc OPC_AND = 12;
    constexpr MachineOpc OPC_OR = 13;
    constexpr MachineOpc OPC_XOR = 14;
    constexpr MachineOpc OPC_SHL = 15;
    constexpr MachineOpc OPC_SHR = 16;
    constexpr MachineOpc OPC_CMP = 17;

    constexpr MachineOpc OPC_MOVE_RI = 18;

    constexpr MachineOpc OPC_SETE = 21;
    constexpr MachineOpc OPC_SETNE = 22;
    constexpr MachineOpc OPC_SETB = 23;
    constexpr MachineOpc OPC_SETNB = 24;
    constexpr MachineOpc OPC_SETL = 25;
    constexpr MachineOpc OPC_SETNL = 26;
    constexpr MachineOpc OPC_SETBE = 27;
    constexpr MachineOpc OPC_SETNBE = 28;
    constexpr MachineOpc OPC_SETLE = 29;
    constexpr MachineOpc OPC_SETNLE = 30;
    constexpr MachineOpc OPC_SETA = 31;
    constexpr MachineOpc OPC_SETNA = 32;
    constexpr MachineOpc OPC_SETG = 33;
    constexpr MachineOpc OPC_SETNG = 34;
    constexpr MachineOpc OPC_SETAE = 35;
    constexpr MachineOpc OPC_SETNAE = 36;
    constexpr MachineOpc OPC_SETGE = 37;
    constexpr MachineOpc OPC_SETNGE = 38;

    constexpr MachineOpc OPC_JMP = 50;
    constexpr MachineOpc OPC_JE = 51;
    constexpr MachineOpc OPC_JNE = 52;
    constexpr MachineOpc OPC_JB = 53;
    constexpr MachineOpc OPC_JNB = 54;
    constexpr MachineOpc OPC_JL = 55;
    constexpr MachineOpc OPC_JNL = 56;
    constexpr MachineOpc OPC_JBE = 57;
    constexpr MachineOpc OPC_JNBE = 58;
    constexpr MachineOpc OPC_JLE = 59;
    constexpr MachineOpc OPC_JNLE = 60;
    constexpr MachineOpc OPC_JA = 61;
    constexpr MachineOpc OPC_JNA = 62;
    constexpr MachineOpc OPC_JG = 63;
    constexpr MachineOpc OPC_JNG = 64;
    constexpr MachineOpc OPC_JAE = 65;
    constexpr MachineOpc OPC_JNAE = 66;
    constexpr MachineOpc OPC_JGE = 67;
    constexpr MachineOpc OPC_JNGE = 68;

    constexpr MachineOpc OPC_PUSH = 101;
    constexpr MachineOpc OPC_POP = 102;
    constexpr MachineOpc OPC_CALL = 103;

    constexpr RegisterID RAX { PREG_START, 64 };
    constexpr RegisterID EAX { RAX.val, 32 };

    constexpr RegisterID RBX { RAX.val + 1, 64 };
    constexpr RegisterID EBX { RBX.val, 32 };

    constexpr RegisterID RCX { RBX.val + 1, 64 };
    constexpr RegisterID ECX { RCX.val, 32 };

    constexpr RegisterID RDX { RCX.val + 1, 64 };
    constexpr RegisterID EDX { RDX.val, 32 };

    constexpr RegisterID RSP { RDX.val + 1, 64 };
    constexpr RegisterID RBP { RSP.val + 1, 64 };

    constexpr RegisterID RSI { RBP.val + 1, 64 };
    constexpr RegisterID ESI { RSI.val, 32 };

    constexpr RegisterID RDI { RSI.val + 1, 64 };
    constexpr RegisterID EDI { RDI.val, 32 };

    constexpr RegisterID R8 { RDI.val + 1, 64 };
    constexpr RegisterID E8 { R8.val, 32 };

    constexpr RegisterID R9 { R8.val + 1, 64 };
    constexpr RegisterID E9 { R9.val + 1, 32 };

    constexpr RegisterID R10 { R9.val + 1, 64 };
    constexpr RegisterID E10 { R10.val, 32 };

    constexpr RegisterID R11 { R10.val + 1, 64 };
    constexpr RegisterID E11 { R11.val, 32 };

    constexpr RegisterID R12 { R11.val + 1, 64 };
    constexpr RegisterID E12 { R12.val, 32 };

    constexpr RegisterID R13 { R12.val + 1, 64 };
    constexpr RegisterID E13 { R13.val, 32 };

    constexpr RegisterID R14 { R13.val + 1, 64 };
    constexpr RegisterID E14 { R14.val, 32 };

    constexpr RegisterID R15 { R14.val + 1, 64 };
    constexpr RegisterID E15 { R15.val, 32 };

    const std::set<RegisterID> CALLER_SAVED_REGS =
    {{
        RAX,
        RBX,
        RCX,
        RDX,
        RSI,
        RDI,
        R8,
        R9,
        R10,
        R11
    }};

    const std::set<RegisterID> CALLEE_SAVED_REGS =
    {
    };

    const RegisterID RETURN_REGISTERS[] = { RAX, RDX };
    const RegisterID ARG_REGISTERS[] = { RDI, RSI, RDX, RCX, R8, R9 };

    class X64Target : public Target
    {
    public:
        std::vector<Pat> load_pats() override;
        std::vector<RegisterClass> load_reg_classes() override;

        bool is_rr_move(MachineOpc opc) override;

        void dump_proc(Procedure& proc, std::ostream& out) override;
        void dump_bblock(BasicBlock& bblock, std::ostream& out) override;
        void abi_lower(Procedure& proc) override;
        void stack_lower(Procedure& proc) override;

        void print_asm(CompileUnit& unit, std::ostream& out, const std::string& src_filename) override;

    private:
        void dump_inst(MachineInstruction& inst, std::ostream& out);
    };
}
