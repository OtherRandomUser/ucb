#pragma once

#include <vector>

#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/isel/pat.hpp>

namespace ucb
{
    struct RegisterClass
    {
        std::vector<RegisterID> physical_regs;
        std::vector<TypeID> tys;
    };

    class Target
    {
    public:
        virtual ~Target() = default;

        virtual std::vector<Pat> load_pats() = 0;
        virtual std::vector<RegisterClass> load_reg_classes() = 0;

        virtual bool is_rr_move(MachineOpc opc) = 0;

        virtual void dump_proc(Procedure& proc, std::ostream& out) = 0;
        virtual void dump_bblock(BasicBlock& bblock, std::ostream& out) = 0;
        virtual void abi_lower(Procedure& proc) = 0;
        virtual void stack_lower(Procedure& proc) = 0;
        // emit prologue/epilogue
    };

    class ASMPrinter
    {
    public:
        virtual ~ASMPrinter() = default;

        virtual void print(CompileUnit& unit, std::ostream& out) = 0;
    };
}
