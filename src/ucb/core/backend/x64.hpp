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

    RegisterID CALLER_SAVED_REGS[] =
    {
        RAX,
        RBX,
        RCX
    };

    RegisterID CALLEE_SAVED_REGS[] =
    {
    };

    class X64Target : public Target
    {
    public:
        std::vector<Pat> load_pats() override
        {
            return std::initializer_list<Pat>
            {
                // add 2 integer registers
                {
                    .cost = 1,
                    .pat = {
                        .kind = PatNode::Inst,
                        .ty = T_I32,
                        .opc = InstrOpcode::OP_ADD,
                        .opnd = OperandKind::OK_POISON,
                        .opnds = {
                            {
                                .kind = PatNode::Opnd,
                                .ty = T_I32,
                                .opc = InstrOpcode::OP_NONE,
                                .opnd = OperandKind::OK_VIRTUAL_REG
                            },
                            {
                                .kind = PatNode::Opnd,
                                .ty = T_I32,
                                .opc = InstrOpcode::OP_NONE,
                                .opnd = OperandKind::OK_VIRTUAL_REG
                            },
                            {
                                .kind = PatNode::Opnd,
                                .ty = T_I32,
                                .opc = InstrOpcode::OP_NONE,
                                .opnd = OperandKind::OK_VIRTUAL_REG
                            }
                        }
                    },
                    .reps = {
                        {
                            .ty = T_I32,
                            .opc = OPC_MOVE,
                            .opnds = {0, 1}
                        },
                        {
                            .ty = T_I32,
                            .opc = OPC_ADD,
                            .opnds = {0, 1, 2}
                        }
                    }
                },
                // return an integer register
                {
                    .cost = 1,
                    .pat = {
                        .kind = PatNode::Inst,
                        .ty = T_I32,
                        .opc = InstrOpcode::OP_RET,
                        .opnd = OperandKind::OK_POISON,
                        .opnds =
                        {
                            {
                                .kind = PatNode::Opnd,
                                .ty = T_I32,
                                .opc = InstrOpcode::OP_NONE,
                                .opnd = OperandKind::OK_VIRTUAL_REG,
                            }
                        }
                    },
                    .reps = {
                        {
                            .ty = T_I32,
                            .opc = OPC_RET,
                            .opnds = {0}
                        }
                    }
                }
            };
        }
    };
}
