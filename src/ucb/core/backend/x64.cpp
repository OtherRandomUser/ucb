#include <ucb/core/backend/x64.hpp>

#include <unordered_map>

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
                        .opc = OPC_MOVE_RM,
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
                        .opc = OPC_MOVE_MR,
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
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_I32,
                        .opc = OPC_ADD_RR,
                        .opnds = {-1, /*0,*/ 1}
                    }
                }
            },
            // add mem with integer register (1)
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
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_I32,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_I32,
                        .opc = OPC_ADD_RM,
                        .opnds = {-1, 1}
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

            case OPC_MOVE_RR:
                out << "mov_rr\t";
                break;

            case OPC_MOVE_MR:
                out << "mov_mr\t";
                break;

            case OPC_MOVE_RM:
                out << "mov_rm\t";
                break;

            case OPC_ADD_RR:
                out << "add_rr\t";
                break;

            case OPC_ADD_MR:
                out << "add_mr\t";
                break;

            case OPC_ADD_RM:
                out << "add_rm\t";
                break;
        }

        auto junc = "";

        static const std::unordered_map<std::uint64_t, std::string> PHYS_REGS = {{
            {std::bit_cast<std::uint64_t>(RAX), "RAX"},
            {std::bit_cast<std::uint64_t>(EAX), "EAX"},
            {std::bit_cast<std::uint64_t>(RBX), "RBX"},
            {std::bit_cast<std::uint64_t>(EBX), "EBX"},
            {std::bit_cast<std::uint64_t>(RCX), "RCX"},
            {std::bit_cast<std::uint64_t>(ECX), "ECX"}
        }};

        for (auto& opnd: inst.opnds)
        {
            out << junc;
            junc = ", ";

            switch (opnd.kind)
            {
                case MachineOperand::Imm:
                    out << "imm(" << opnd.val << ")";
                    break;

                case MachineOperand::Register: {
                    auto it = PHYS_REGS.find(opnd.val);

                    if (it == PHYS_REGS.end())
                    {
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        out << "reg({ " << reg.val << ", " << reg.size << " })";
                    }
                    else
                    {
                        out << it->second;
                    }

                    break;
                }

                case MachineOperand::FrameSlot:
                    out << "fs(#" << std::bit_cast<std::int64_t>(opnd.val) << ")";
                    break;

                case MachineOperand::BBlockAddress:
                    out << "bb(" << opnd.val << ")";
                    break;
            }
        }

        out << "\n";
    }

    void X64Target::abi_lower(Procedure& proc)
    {
        // lower inputs

        auto& params = proc.params();
        auto& entry = proc.entry();

        auto in_reg = 0;
        std::int64_t in_slot = -1;
        auto total_args = params.size();

        // TODO actually check the ABI and add remaining registers
        std::vector<RegisterID> regs = {RAX, RBX, RCX};

        // lower inputs
        for (const auto& [reg, vreg]: params)
        {
            if (in_reg >= regs.size())
            {
                // pass via frame
                auto ty = vreg.ty();
                auto& inst = entry.prepend_machine_instr(OPC_MOVE_RM/*, vreg.ty()*/);

                inst.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = ty,
                    .val = std::bit_cast<std::uint64_t>(reg),
                    .is_def = true
                });

                inst.opnds.push_back({
                    .kind = MachineOperand::FrameSlot,
                    .ty = ty,
                    .val = std::bit_cast<std::uint64_t>(in_slot--)
                });
            }
            else
            {
                // pass via register
                auto ty = vreg.ty();
                auto& inst = entry.prepend_machine_instr(OPC_MOVE_RR);

                RegisterID p_reg = regs[in_reg++];
                p_reg.size = ty.size;

                inst.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = ty,
                    .val = std::bit_cast<std::uint64_t>(reg),
                    .is_def = true
                });

                inst.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = ty,
                    .val = std::bit_cast<std::uint64_t>(p_reg)
                });
            }
        }

        // lower outputs
        for (auto& bblock: proc.bblocks())
        {
            auto& machine_insts = bblock.machine_insts();

            if (machine_insts.back().opc == OPC_RET)
            {
                auto out_ret = machine_insts.back().opnds.front();
                machine_insts.pop_back();

                RegisterID reg = RAX;
                reg.size = out_ret.ty.size;

                auto& mov = machine_insts.emplace_back(OPC_MOVE_RR);
                mov.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = out_ret.ty,
                    .val = std::bit_cast<std::uint64_t>(reg)
                });
                mov.opnds.push_back(out_ret);

                auto& ret = machine_insts.emplace_back(OPC_RET);
                ret.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = out_ret.ty,
                    .val = std::bit_cast<std::uint64_t>(reg)
                });
            }
        }
    }
}
