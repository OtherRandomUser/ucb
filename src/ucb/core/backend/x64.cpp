#include <ucb/core/backend/x64.hpp>

namespace ucb::x64
{
    std::vector<Pat> X64Target::load_pats()
    {
        return std::initializer_list<Pat>
        {
            // single load from frame slot
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_I32,
                    .opc = InstrOpcode::OP_LOAD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_I32,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_FRAME_SLOT
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_I32,
                        .opc = OPC_MOVE,
                        .opnds = {-1, 0}
                    }
                }
            },
            // single store to frame slot
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_I32,
                    .opc = InstrOpcode::OP_STORE,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_I32,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_FRAME_SLOT
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
                        // .opnds = {-1, 0}
                        .opnds = {0, 1}
                    }
                }
            },
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
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_I32,
                        .opc = OPC_MOVE,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_I32,
                        .opc = OPC_ADD,
                        .opnds = {-1, /*0,*/ 1}
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
                /*
                .reps = {
                    {
                        .ty = T_I32,
                        .opc = OPC_MOVE,
                        .opnds = {EAX, 0}
                    },
                    {
                        .ty = T_I32,
                        .opc = OPC_RET,
                        .opnds = {EAX}
                    }
                }
                */
            }
        };
    }

    void X64Target::dump_bblock(BasicBlock& bblock, std::ostream& out)
    {
        out << bblock.id() << ":\n";

        if (!bblock.predecessors().empty())
        {
            out << "\tpredecessors:\n";

            for (auto pred: bblock.predecessors())
            {
                out << "\t\t" << pred->id() << "\n";
            }

            out << "\n";
        }

        if (!bblock.live_ins().empty())
        {
            out << "\tlive ins:\n";

            for (auto [id, ty]: bblock.live_ins())
            {
                out << "\t\t";
                bblock.context()->dump_ty(out, ty);
                out << " { " << id.val << ", " << id.size << " }\n";
            }

            out << "\n";
        }

        for (auto& inst: bblock.machine_insts())
        {
            dump_inst(inst, out);
        }

        if (!bblock.live_outs().empty())
        {
            out << "live outs:\n";

            for (auto [id, ty]: bblock.live_outs())
            {
                out << "\t\t";
                bblock.context()->dump_ty(out, ty);
                out << " { " << id.val << ", " << id.size << " }\n";
            }

            out << "\n";
        }

        out << "\n";
    }

    void X64Target::dump_inst(MachineInstruction& inst, std::ostream& out)
    {
        out << "\t";

        switch (inst.opc)
        {
            default:
                std::cerr << "unreachable" << std::endl;
                abort();

            case OPC_RET:
                out << "ret\t";
                break;

            case OPC_MOVE:
                out << "mov\t";
                break;

            case OPC_ADD:
                out << "add\t";
                break;
        }

        auto junc = "";

        for (auto& opnd: inst.opnds)
        {
            out << junc;
            junc = ", ";
            auto reg = NO_REG;

            switch (opnd.kind)
            {
                case MachineOperand::Imm:
                    out << "imm(" << opnd.val << ")";
                    break;

                case MachineOperand::Register:
                    reg = std::bit_cast<RegisterID>(opnd.val);
                    out << "reg({ " << reg.val << ", " << reg.size << " })";
                    break;

                case MachineOperand::FrameSlot:
                    out << "fs(#" << opnd.val << ")";
                    break;

                case MachineOperand::BBlockAddress:
                    out << "bb(" << opnd.val << ")";
                    break;
            }
        }

        out << "\n";
    }
}
