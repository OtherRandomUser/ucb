#include <ucb/core/backend/x64.hpp>

#include <set>
#include <unordered_map>

namespace ucb::x64
{
    static const std::unordered_map<MachineOpc, std::string> OPCS = {{
        {OPC_NONE, "\tNONE\t"},
        {OPC_RET, "\tret"},
        {OPC_MOVE_RR, "\tmov\t"},
        {OPC_MOVE_MR, "\tmov\t"},
        {OPC_MOVE_RM, "\tmov\t"},
        {OPC_ADD, "\tadd\t"},
        {OPC_SUB, "\tsub\t"},
        {OPC_JMP, "\tjmp\t"},
        {OPC_PUSH, "\tpush\t"},
        {OPC_POP, "\tpop\t"},
    }};

    static const std::unordered_map<std::uint64_t, std::string> PHYS_REGS = {{
        {std::bit_cast<std::uint64_t>(RAX), "rax"},
        {std::bit_cast<std::uint64_t>(EAX), "eax"},
        {std::bit_cast<std::uint64_t>(RBX), "rbx"},
        {std::bit_cast<std::uint64_t>(EBX), "ebx"},
        {std::bit_cast<std::uint64_t>(RCX), "rcx"},
        {std::bit_cast<std::uint64_t>(ECX), "ecx"},
        {std::bit_cast<std::uint64_t>(RDX), "rdx"},
        {std::bit_cast<std::uint64_t>(EDX), "edx"},
        {std::bit_cast<std::uint64_t>(RSP), "rsp"},
        {std::bit_cast<std::uint64_t>(RBP), "rbp"},
        {std::bit_cast<std::uint64_t>(RSI), "rsi"},
        {std::bit_cast<std::uint64_t>(ESI), "esi"},
        {std::bit_cast<std::uint64_t>(RDI), "rdi"},
        {std::bit_cast<std::uint64_t>(EDI), "edi"},
        {std::bit_cast<std::uint64_t>(R8),  "r8"},
        {std::bit_cast<std::uint64_t>(E8),  "e8"},
        {std::bit_cast<std::uint64_t>(R9),  "r9"},
        {std::bit_cast<std::uint64_t>(E9),  "e9"},
        {std::bit_cast<std::uint64_t>(R10), "r10"},
        {std::bit_cast<std::uint64_t>(E10), "e10"},
        {std::bit_cast<std::uint64_t>(R11), "r11"},
        {std::bit_cast<std::uint64_t>(E11), "e11"},
        {std::bit_cast<std::uint64_t>(R12), "r12"},
        {std::bit_cast<std::uint64_t>(E12), "e12"},
        {std::bit_cast<std::uint64_t>(R13), "r13"},
        {std::bit_cast<std::uint64_t>(E13), "e13"},
        {std::bit_cast<std::uint64_t>(R14), "r14"},
        {std::bit_cast<std::uint64_t>(E14), "e14"},
        {std::bit_cast<std::uint64_t>(R15), "r15"},
        {std::bit_cast<std::uint64_t>(E15), "e15"},
    }};

    std::vector<Pat> X64Target::load_pats()
    {
        return std::initializer_list<Pat>
        {
            // single load signed int from frame slot
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_LOAD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_FRAME_SLOT
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RM,
                        .opnds = {-1, 0}
                    }
                }
            },
            // single load unsigned int from frame slot
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_LOAD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_FRAME_SLOT
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RM,
                        .opnds = {-1, 0}
                    }
                }
            },
            // single store signed int to frame slot
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_STORE,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_FRAME_SLOT
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_MR,
                        // .opnds = {-1, 0}
                        .opnds = {0, 1}
                    }
                }
            },
            // single store unsigned int to frame slot
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_STORE,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_FRAME_SLOT
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_MR,
                        // .opnds = {-1, 0}
                        .opnds = {0, 1}
                    }
                }
            },
            // add 2 signed integer registers
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_ADD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_ADD,
                        .opnds = {-1, /*0,*/ 1},
                        .def_is_also_use = true
                    }
                }
            },
            // add 2 unsigned integer registers
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_ADD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_ADD,
                        .opnds = {-1, /*0,*/ 1},
                        .def_is_also_use = true
                    }
                }
            },
            // add mem with signed integer register (1)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_ADD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_ADD,
                        .opnds = {-1, 1},
                        .def_is_also_use = true
                    }
                }
            },
            // add mem with signed integer register (2)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_ADD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 1}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_ADD,
                        .opnds = {-1, 0},
                        .def_is_also_use = true
                    }
                }
            },
            // add mem with unsigned integer register (1)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_ADD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_ADD,
                        .opnds = {-1, 1},
                        .def_is_also_use = true
                    }
                }
            },
            // add mem with unsigned integer register (2)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_ADD,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 1}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_ADD,
                        .opnds = {-1, 0},
                        .def_is_also_use = true
                    }
                }
            },
            // sub 2 signed integer registers
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_SUB,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_SUB,
                        .opnds = {-1, 1},
                        .def_is_also_use = true
                    }
                }
            },
            // sub 2 unsigned integer registers
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_SUB,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_SUB,
                        .opnds = {-1, 1},
                        .def_is_also_use = true
                    }
                }
            },
            // sub mem with signed integer register (1)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_SUB,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_SUB,
                        .opnds = {-1, 1},
                        .def_is_also_use = true
                    }
                }
            },
            // sub mem with signed integer register (2)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_SUB,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 1}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_SUB,
                        .opnds = {-1, 0},
                        .def_is_also_use = true
                    }
                }
            },
            // sub mem with unsigned integer register (1)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_SUB,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        },
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 0}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_SUB,
                        .opnds = {-1, 1},
                        .def_is_also_use = true
                    }
                }
            },
            // sub mem with unsigned integer register (2)
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_SUB,
                    .opnd = OperandKind::OK_POISON,
                    .opnds = {
                        {
                            .kind = PatNode::Inst,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_LOAD,
                            .opnd = OperandKind::OK_POISON,
                            .opnds = {
                                {
                                    .kind = PatNode::Opnd,
                                    .ty = T_SAME,
                                    .opc = InstrOpcode::OP_NONE,
                                    .opnd = OperandKind::OK_FRAME_SLOT
                                }
                            }
                        },
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_MOVE_RR,
                        .opnds = {-1, 1}
                    },
                    {
                        .ty = T_SAME,
                        .opc = OPC_SUB,
                        .opnds = {-1, 0},
                        .def_is_also_use = true
                    }
                }
            },
            // return an integer register
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_I,
                    .opc = InstrOpcode::OP_RET,
                    .opnd = OperandKind::OK_POISON,
                    .opnds =
                    {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG,
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_RET,
                        .opnds = {0}
                    }
                }
            },
            // return an unsigned integer register
            {
                .cost = 1,
                .pat = {
                    .kind = PatNode::Inst,
                    .ty = T_ANY_U,
                    .opc = InstrOpcode::OP_RET,
                    .opnd = OperandKind::OK_POISON,
                    .opnds =
                    {
                        {
                            .kind = PatNode::Opnd,
                            .ty = T_SAME,
                            .opc = InstrOpcode::OP_NONE,
                            .opnd = OperandKind::OK_VIRTUAL_REG,
                        }
                    }
                },
                .reps = {
                    {
                        .ty = T_SAME,
                        .opc = OPC_RET,
                        .opnds = {0}
                    }
                }
            }
        };
    }

    std::vector<RegisterClass> X64Target::load_reg_classes()
    {
        return std::initializer_list<RegisterClass>
        {
            // GPR
            {
                .physical_regs = {RAX, RBX, RCX},
                .tys = {T_ANY_I, T_ANY_U}
            }
        };
    }

    bool X64Target::is_rr_move(MachineOpc opc)
    {
        return opc == OPC_MOVE_RR;
    }

    void X64Target::dump_proc(Procedure& proc, std::ostream& out)
    {
        proc.context()->dump_ty(out, proc.signature().ret());
        out << " @" << proc.id();

        out << "\n{\n";

        for (auto& bblock: proc.bblocks())
        {
            dump_bblock(bblock, out);
        }

        out << "}\n";
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

            case OPC_ADD:
                out << "add\t";
                break;

            case OPC_SUB:
                out << "sub\t";
                break;

            case OPC_JMP:
                out << "jmp\t";
                break;

            case OPC_PUSH:
                out << "push\t";
                break;

            case OPC_POP:
                out << "pop\t";
                break;
        }

        auto junc = std::to_string(inst.size) + "\t";

        for (auto& opnd: inst.opnds)
        {
            out << junc;
            junc = ", ";

            switch (opnd.kind)
            {
                case MachineOperand::Imm: {
                    auto val = static_cast<std::int64_t>(opnd.val);
                    out << "imm(" << val << ")";
                    break;
                }

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

                case MachineOperand::MemAddr: {
                    auto it = PHYS_REGS.find(opnd.val);

                    if (it == PHYS_REGS.end())
                    {
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        out << "[{ " << reg.val << ", " << reg.size << " }";
                    }
                    else
                    {
                        out << "[" << it->second;
                    }

                    if (opnd.offset < 0)
                    {
                        out << " - " << -opnd.offset << "]";
                    }
                    else
                    {
                        out << " + " << opnd.offset << "]";
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
        std::vector<RegisterID> regs = {RDI, RSI, RDX, RCX, R8};

        // lower inputs
        for (const auto& [reg, vreg]: params)
        {
            if (in_reg >= regs.size())
            {
                // pass via frame
                auto ty = vreg.ty();
                auto& inst = entry.prepend_machine_instr(OPC_MOVE_RM);
                inst.size = vreg.ty().size;

                inst.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = ty,
                    .val = std::bit_cast<std::uint64_t>(reg),
                    .is_def = true,
                    .is_use = false
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
                inst.size = ty.size;

                RegisterID p_reg = regs[in_reg++];
                p_reg.size = ty.size;

                inst.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = ty,
                    .val = std::bit_cast<std::uint64_t>(reg),
                    .is_def = true,
                    .is_use = false
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
                mov.size = reg.size;
                mov.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = out_ret.ty,
                    .val = std::bit_cast<std::uint64_t>(reg),
                    .is_def = true,
                    .is_use = false
                });
                mov.opnds.push_back(out_ret);

                auto& ret = machine_insts.emplace_back(OPC_RET);
                /*
                ret.opnds.push_back({
                    .kind = MachineOperand::Register,
                    .ty = out_ret.ty,
                    .val = std::bit_cast<std::uint64_t>(reg)
                });*/
            }
        }
    }

    void X64Target::stack_lower(Procedure& proc)
    {
        // compute functionwise clobbers
        proc.compute_machine_lifetimes();
        std::set<RegisterID> callee_saved_clobbers;

        for (auto& bblock: proc.bblocks())
        {
            for (auto& inst: bblock.machine_insts())
            {
                for (auto& opnd: inst.opnds)
                {
                    if (opnd.kind == MachineOperand::Register)
                    {
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        reg.size = 64; // little trick to make all sizes work

                        if (CALLEE_SAVED_REGS.contains(reg))
                        {
                            callee_saved_clobbers.insert(reg);
                        }
                    }
                }
            }
        }

        // add stackup
        auto stackup_insertpoint = proc.entry().machine_insts().begin();

        // calculate stack offsets and stack size
        auto stack_size = 0;
        auto leftover_padding = 0;
        auto last_offset = 0;
        auto alignment = 2; // 16 bits, might be raised if floats are used
        std::vector<int> stack_offsets;

        for (auto& [reg, vr]: proc.frame())
        {
            int byte_size = reg.size / 8;

            if (leftover_padding >= byte_size)
            {
                last_offset += byte_size;
                leftover_padding -= byte_size;
                stack_offsets.push_back(last_offset);
            }
            else
            {
                stack_size += byte_size;
                last_offset = stack_size;
                stack_offsets.push_back(last_offset);
                auto mod = stack_size % alignment;

                if (mod > 0)
                {
                    leftover_padding = alignment - mod;
                    stack_size += leftover_padding;
                }
                else
                {
                    leftover_padding = 0;
                }
            }
        }

        if (stack_size > 0)
        {
            // save the old rbp
            MachineInstruction push_rbp;
            push_rbp.opc = OPC_PUSH;
            push_rbp.size = RBP.size;
            push_rbp.opnds.push_back({
                .kind = MachineOperand::Register,
                .val = std::bit_cast<std::uint64_t>(RBP)
            });
            proc.entry().machine_insts().insert(stackup_insertpoint, std::move(push_rbp));

            // update rbp
            MachineInstruction update_rbp;
            update_rbp.opc = OPC_MOVE_RR;
            update_rbp.size = RBP.size;
            update_rbp.opnds.push_back({
                .kind = MachineOperand::Register,
                .val = std::bit_cast<std::uint64_t>(RBP)
            });
            update_rbp.opnds.push_back({
                .kind = MachineOperand::Register,
                .val = std::bit_cast<std::uint64_t>(RSP)
            });
            proc.entry().machine_insts().insert(stackup_insertpoint, std::move(update_rbp));

            // increase stack
            MachineInstruction update_rsp;
            update_rsp.opc = OPC_SUB;
            update_rsp.size = RSP.size;
            auto ty = T_ANY_I;
            ty.size = RSP.size;
            update_rsp.opnds.push_back({
                .kind = MachineOperand::Register,
                .ty = ty,
                .val = std::bit_cast<std::uint64_t>(RSP)
            });
            update_rsp.opnds.push_back({
                .kind = MachineOperand::Imm,
                .ty = ty,
                .val = static_cast<std::uint64_t>(stack_size)
            });
            proc.entry().machine_insts().insert(stackup_insertpoint, std::move(update_rsp));
        }

        // push calle saved registers, doing this now will keep rbp intact
        for (auto reg: callee_saved_clobbers)
        {
            MachineInstruction inst;
            inst.opc = OPC_PUSH;
            inst.size = reg.size;

            MachineOperand opnd;
            opnd.kind = MachineOperand::Register;
            opnd.val = std::bit_cast<std::uint64_t>(reg);
            inst.opnds.push_back(opnd);

            proc.entry().machine_insts().insert(stackup_insertpoint, std::move(inst));
        }

        if (stack_size > 0 || callee_saved_clobbers.size() > 0)
        {
            auto exit_id = -1;

            // compute ret count
            std::set<int> rets;
            auto& bblocks = proc.bblocks();

            for (int i = 0; i < bblocks.size(); ++i)
            {
                auto& inst = bblocks[i].machine_insts().back();

                if (inst.opc == OPC_RET)
                {
                    inst.opnds.clear();
                    rets.insert(i);
                }
            }

            // if single ret, add stackdown before it
            if (rets.size() == 1)
            {
                exit_id = *rets.begin();
            }
            // otherwise, add stackdown on new basic block
            else if (rets.size() > 0)
            {
                auto exit_id = proc.add_bblock("exit");

                for (auto id: rets)
                {
                    auto& bblock = bblocks[id];
                    bblock.machine_insts().pop_back();

                    MachineInstruction inst;
                    inst.opc = OPC_JMP;

                    MachineOperand opnd;
                    opnd.kind = MachineOperand::BBlockAddress;
                    opnd.val = static_cast<std::uint64_t>(exit_id);
                    inst.opnds.push_back(opnd);

                    bblock.machine_insts().push_back(std::move(inst));

                }

                auto& exit = bblocks[exit_id];

                MachineInstruction inst;
                inst.opc = OPC_RET;

                MachineOperand opnd;
                opnd.kind = MachineOperand::BBlockAddress;
                opnd.val = std::bit_cast<std::uint64_t>(RAX);
                inst.opnds.push_back(opnd);

                exit.machine_insts().push_back(std::move(inst));

            }
            else
            {
                std::cerr << "unreachable" << std::endl;
                abort();
            }

            auto& exit = bblocks[exit_id];
            auto stackdown_insertpoint = --exit.machine_insts().end();

            // pop calle saved registers in reverse order
            for (auto csc = callee_saved_clobbers.rbegin(); csc != callee_saved_clobbers.rend(); ++csc)
            {
                MachineInstruction inst;
                inst.opc = OPC_POP;
                inst.size = csc->size;

                MachineOperand opnd;
                opnd.kind = MachineOperand::Register;
                opnd.val = std::bit_cast<std::uint64_t>(*csc);
                inst.opnds.push_back(opnd);

                proc.entry().machine_insts().insert(stackdown_insertpoint, std::move(inst));
            }

            // restore rsp
            MachineInstruction restore_rsp;
            restore_rsp.opc = OPC_ADD;
            restore_rsp.size = RSP.size;
            auto ty = T_ANY_I;
            ty.size = RSP.size;
            restore_rsp.opnds.push_back({
                .kind = MachineOperand::Register,
                .ty = ty,
                .val = std::bit_cast<std::uint64_t>(RSP)
            });
            restore_rsp.opnds.push_back({
                .kind = MachineOperand::Imm,
                .ty = ty,
                .val = static_cast<std::uint64_t>(stack_size)
            });
            proc.entry().machine_insts().insert(stackdown_insertpoint, std::move(restore_rsp));

            // restore rsp
            MachineInstruction restore_rbp;
            restore_rbp.opc = OPC_POP;
            restore_rbp.size = RBP.size;
            restore_rbp.opnds.push_back({
                .kind = MachineOperand::Register,
                .val = std::bit_cast<std::uint64_t>(RBP)
            });
            proc.entry().machine_insts().insert(stackdown_insertpoint, std::move(restore_rbp));
        }

        // for each function call add stackup & stackdown
        for (auto& bblock: proc.bblocks())
        {
            for (auto& inst: bblock.machine_insts())
            {
                // TODO stack up & down for calls after adding call instructions
                //

                // replace slot indexes with stack offsts
                auto it = inst.opnds.begin();
                while (it != inst.opnds.end())
                {
                    auto& opnd = *it;

                    if (opnd.kind == MachineOperand::FrameSlot)
                    {
                        auto slot = opnd.val;

                        if (slot >= stack_offsets.size())
                        {
                            std::cerr << "invalid stack slot" << std::endl;
                            abort();
                        }

                        auto offset = stack_offsets[slot];

                        it = ++inst.opnds.insert(it, {
                            .kind = MachineOperand::MemAddr,
                            .val = std::bit_cast<std::uint64_t>(RBP),
                            .offset = -offset
                        });

                        it = inst.opnds.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
    }

    void X64Target::print_asm(CompileUnit& unit, std::ostream& out, const std::string& src_filename)
    {
        out
            << "\t.text\n"
            << "\t.intel_syntax noprefix\n"
            << "\t.file   \"" << src_filename << "\"\n";

        int proc_cnt = 0;
        int tmp_lbl_cnt = 0;

        for (auto proc_ptr: unit.procs())
        {
            auto& proc = *proc_ptr;

            out
                << "\t.globl\t" << proc.id() << "\n"
                << "\t.p2align\t4, 0x90\n"
                << "\t.type\t" << proc.id() << ",@function\n"
                << proc.id() << ":\n";

            std::vector<std::string> tmp_lbl_names;
            tmp_lbl_names.reserve(proc.bblocks().size());

            for (auto& bblock: proc.bblocks())
            {
                std::string name = ".TmpLbl" + std::to_string(tmp_lbl_cnt++);
                tmp_lbl_names.push_back(std::move(name));
            }

            for (int i = 0; i < proc.bblocks().size(); ++i)
            {
                auto& bblock = proc.bblocks()[i];
                out << tmp_lbl_names[i] << ": # bblock " << bblock.id() << "\n";

                for (auto& inst: bblock.machine_insts())
                {
                    out << OPCS.at(inst.opc);
                    std::string junc = "";

                    for (auto& opnd: inst.opnds)
                    {
                        switch (opnd.kind)
                        {
                            case MachineOperand::Imm:
                                if (opnd.ty.val == T_ANY_U.val)
                                {
                                    out << junc << opnd.val;
                                }
                                else if (opnd.ty.val == T_ANY_I.val)
                                {
                                    out << junc << static_cast<std::int64_t>(opnd.val);
                                }
                                else if (opnd.ty.val == T_ANY_F.val)
                                {
                                    out << junc << std::bit_cast<double>(opnd.val);
                                }
                                else
                                {
                                    std::cerr << "unexpected type" << std::endl;
                                    abort();
                                }

                                break;

                            case MachineOperand::Register:
                                out << junc << PHYS_REGS.at(opnd.val);
                                break;

                            case MachineOperand::MemAddr:
                                out << junc;

                                switch (inst.size)
                                {
                                default:
                                    std::cerr << "unexpected size: " << inst.size << std::endl;
                                    abort();

                                case  8: out << "hword ptr ["; break;
                                case 16: out <<  "word ptr ["; break;
                                case 32: out << "dword ptr ["; break;
                                case 64: out << "qword ptr ["; break;
                                }

                                out << PHYS_REGS.at(opnd.val);

                                if (opnd.offset < 0)
                                {
                                    out << " - " << -opnd.offset;
                                }
                                else
                                {
                                    out << " + " << opnd.offset;
                                }

                                out << "]";
                                break;

                            case MachineOperand::FrameSlot:
                                std::cerr << "unreachable" << std::endl;
                                abort();

                            case MachineOperand::BBlockAddress:
                                out << junc << tmp_lbl_names[opnd.val];
                                break;
                        }

                        junc = ", ";
                    }

                    out << "\n";
                }
            }

            auto end_lbl = ".TmpProcEnd" + std::to_string(proc_cnt++);
            out
                << end_lbl << ":\n"
                << "\t.size " << proc.id() << ", " << end_lbl << "-" << proc.id() << std::endl;
        }

        out.flush();
    }
}
