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
        {OPC_MOVE_RI, "\tmov\t"},
        {OPC_ADD, "\tadd\t"},
        {OPC_SUB, "\tsub\t"},
        {OPC_MUL, "\tmul\t"},
        {OPC_IMUL, "\timul\t"},
        {OPC_DIV, "\tdiv\t"},
        {OPC_IDIV, "\tidiv\t"},
        {OPC_NOT, "\tnot\t"},
        {OPC_AND, "\tand\t"},
        {OPC_OR, "\tor\t"},
        {OPC_XOR, "\txor\t"},
        {OPC_SHL, "\tshl\t"},
        {OPC_SHR, "\tshr\t"},
        {OPC_CMP, "\tcmp\t"},
        {OPC_SETE, "\tsete\t"},
        {OPC_SETNE, "\tsetne\t"},
        {OPC_SETB, "\tsetb\t"},
        {OPC_SETNB, "\tsetnb\t"},
        {OPC_SETL, "\tsetl\t"},
        {OPC_SETNL, "\tsetnl\t"},
        {OPC_SETBE, "\tsetbe\t"},
        {OPC_SETNBE, "\tsetnbe\t"},
        {OPC_SETLE, "\tsetle\t"},
        {OPC_SETNLE, "\tsetnle\t"},
        {OPC_SETA, "\tseta\t"},
        {OPC_SETNA, "\tsetna\t"},
        {OPC_SETG, "\tsetg\t"},
        {OPC_SETNG, "\tsetng\t"},
        {OPC_SETAE, "\tsetae\t"},
        {OPC_SETNAE, "\tsetnae\t"},
        {OPC_SETGE, "\tsetge\t"},
        {OPC_SETNGE, "\tsetnge\t"},
        {OPC_JMP, "\tjmp\t"},
        {OPC_JE, "\tje\t"},
        {OPC_JNE, "\tjne\t"},
        {OPC_JB, "\tjb\t"},
        {OPC_JNB, "\tjnb\t"},
        {OPC_JL, "\tjl\t"},
        {OPC_JNL, "\tjnl\t"},
        {OPC_JBE, "\tjbe\t"},
        {OPC_JNBE, "\tjnbe\t"},
        {OPC_JLE, "\tjle\t"},
        {OPC_JNLE, "\tjnle\t"},
        {OPC_JA, "\tja\t"},
        {OPC_JNA, "\tjna\t"},
        {OPC_JG, "\tjg\t"},
        {OPC_JNG, "\tjng\t"},
        {OPC_JAE, "\tjae\t"},
        {OPC_JNAE, "\tjnae\t"},
        {OPC_JGE, "\tjge\t"},
        {OPC_JNGE, "\tjnge\t"},
        {OPC_PUSH, "\tpush\t"},
        {OPC_POP, "\tpop\t"},
        {OPC_CALL, "\tcall\t"}
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

#define PAT_REG_OPND                        \
    {                                       \
        .kind = PatNode::Opnd,              \
        .ty = T_SAME,                       \
        .opc = InstrOpcode::OP_NONE,        \
        .opnd = OperandKind::OK_VIRTUAL_REG \
    }

#define PAT_REG_OPND_T(TY)                  \
    {                                       \
        .kind = PatNode::Opnd,              \
        .ty = TY,                           \
        .opc = InstrOpcode::OP_NONE,        \
        .opnd = OperandKind::OK_VIRTUAL_REG \
    }

#define PAT_SLOT_OPND                       \
    {                                       \
        .kind = PatNode::Opnd,              \
        .ty = T_SAME,                       \
        .opc = InstrOpcode::OP_NONE,        \
        .opnd = OperandKind::OK_FRAME_SLOT  \
    }

#define PAT_BBLOCK_OPND                     \
    {                                       \
        .kind = PatNode::Opnd,              \
        .ty = T_STATIC_ADDRESS,             \
        .opc = InstrOpcode::OP_NONE,        \
        .opnd = OperandKind::OK_BASIC_BLOCK \
    }

#define PAT_IMM_INT_OPND                        \
    {                                           \
        .kind = PatNode::Opnd,                  \
        .ty = T_SAME,                           \
        .opc = InstrOpcode::OP_NONE,            \
        .opnd = OperandKind::OK_INTEGER_CONST   \
    }

#define INST_PAT_NODE(TY, OP, ...)      \
    {                                   \
        .kind = PatNode::Inst,          \
        .ty = TY,                       \
        .opc = InstrOpcode::OP,         \
        .opnd = OperandKind::OK_POISON, \
        .opnds = { __VA_ARGS__ }        \
    }

#define INST_PAT_NODE_ID(TY, OP, ID, ...)   \
    {                                       \
        .kind = PatNode::Inst,              \
        .ty = TY,                           \
        .opc = InstrOpcode::OP,             \
        .opnd = OperandKind::OK_POISON,     \
        .id = #ID,                          \
        .opnds = { __VA_ARGS__ }            \
    }

#define LOAD_PAT_NODE(TY)           INST_PAT_NODE(TY, OP_LOAD, PAT_SLOT_OPND)
#define RET_PAT_NODE(TY, ARG)       INST_PAT_NODE(TY, OP_RET, ARG) 
#define ADD_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_ADD, LHS, RHS)
#define SUB_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_SUB, LHS, RHS)
#define MUL_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_MUL, LHS, RHS)
#define NOT_PAT_NODE(TY, ARG)       INST_PAT_NODE(TY, OP_NOT, ARG)
#define MUL_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_MUL, LHS, RHS)
#define AND_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_AND, LHS, RHS)
#define OR_PAT_NODE(TY, LHS, RHS)   INST_PAT_NODE(TY, OP_OR, LHS, RHS)
#define XOR_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_XOR, LHS, RHS)
#define SHL_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_SHL, LHS, RHS)
#define SHR_PAT_NODE(TY, LHS, RHS)  INST_PAT_NODE(TY, OP_SHR, LHS, RHS)

#define CMP_EQ_PAT_NODE(LHS, RHS)  INST_PAT_NODE_ID(T_BOOL, OP_CMP, eq, LHS, RHS)
#define CMP_NE_PAT_NODE(LHS, RHS)  INST_PAT_NODE_ID(T_BOOL, OP_CMP, ne, LHS, RHS)
#define CMP_LT_PAT_NODE(LHS, RHS)  INST_PAT_NODE_ID(T_BOOL, OP_CMP, lt, LHS, RHS)
#define CMP_LE_PAT_NODE(LHS, RHS)  INST_PAT_NODE_ID(T_BOOL, OP_CMP, le, LHS, RHS)
#define CMP_GT_PAT_NODE(LHS, RHS)  INST_PAT_NODE_ID(T_BOOL, OP_CMP, gt, LHS, RHS)
#define CMP_GE_PAT_NODE(LHS, RHS)  INST_PAT_NODE_ID(T_BOOL, OP_CMP, ge, LHS, RHS)

#define BR_PAT_NODE(ARG)        INST_PAT_NODE(T_STATIC_ADDRESS, OP_BR, ARG)
#define BRC_PAT_NODE(CND, T, F) INST_PAT_NODE(T_STATIC_ADDRESS, OP_BRC, CND, T, F)

#define CALL_PAT_NODE(TY)               \
    {                                   \
        .kind = PatNode::Inst,          \
        .ty = TY,                       \
        .opc = InstrOpcode::OP_CALL,    \
        .opnd = OperandKind::OK_POISON, \
        .is_va_pat = true               \
    }

#define REP_NODE(TY, OPC, ...)  \
    {                           \
        .ty = TY,               \
        .opc = OPC,             \
        .opnds = {__VA_ARGS__}  \
    }

#define BIN_NODE(TY, OPC, ...)      \
    {                               \
        .ty = TY,                   \
        .opc = OPC,                 \
        .opnds = {__VA_ARGS__} ,    \
        .def_is_also_use = true     \
    }

#define REP_CALL(TY)        \
    {                       \
        .ty = TY,           \
        .opc = OPC_CALL,    \
        .is_va_rep = true   \
    }

#define REP_MOVE_RM(...) REP_NODE(T_SAME, OPC_MOVE_RM, __VA_ARGS__)
#define REP_MOVE_MR(...) REP_NODE(T_SAME, OPC_MOVE_MR, __VA_ARGS__)
#define REP_MOVE_RR(...) REP_NODE(T_SAME, OPC_MOVE_RR, __VA_ARGS__)
#define REP_RET(...)     REP_NODE(T_SAME, OPC_RET,     __VA_ARGS__)
#define REP_ADD(...)     BIN_NODE(T_SAME, OPC_ADD,     __VA_ARGS__)
#define REP_SUB(...)     BIN_NODE(T_SAME, OPC_SUB,     __VA_ARGS__)
#define REP_MUL(...)     BIN_NODE(T_SAME, OPC_MUL,     __VA_ARGS__)
#define REP_IMUL(...)    BIN_NODE(T_SAME, OPC_IMUL,    __VA_ARGS__)
#define REP_NOT(...)     BIN_NODE(T_SAME, OPC_NOT,     __VA_ARGS__)
#define REP_AND(...)     BIN_NODE(T_SAME, OPC_AND,     __VA_ARGS__)
#define REP_OR(...)      BIN_NODE(T_SAME, OPC_OR,      __VA_ARGS__)
#define REP_XOR(...)     BIN_NODE(T_SAME, OPC_XOR,     __VA_ARGS__)
#define REP_SHL(...)     BIN_NODE(T_SAME, OPC_SHL,     __VA_ARGS__)
#define REP_SHR(...)     BIN_NODE(T_SAME, OPC_SHR,     __VA_ARGS__)
#define REP_CMP(...)     REP_NODE(T_SAME, OPC_CMP,     __VA_ARGS__)

#define REP_SETE(...)    REP_NODE(T_BOOL, OPC_SETE,    __VA_ARGS__)
#define REP_SETNE(...)   REP_NODE(T_BOOL, OPC_SETNE,   __VA_ARGS__)
#define REP_SETB(...)    REP_NODE(T_BOOL, OPC_SETB,    __VA_ARGS__)
#define REP_SETNB(...)   REP_NODE(T_BOOL, OPC_SETNB,   __VA_ARGS__)
#define REP_SETL(...)    REP_NODE(T_BOOL, OPC_SETL,    __VA_ARGS__)
#define REP_SETNL(...)   REP_NODE(T_BOOL, OPC_SETNL,   __VA_ARGS__)
#define REP_SETBE(...)   REP_NODE(T_BOOL, OPC_SETBE,   __VA_ARGS__)
#define REP_SETNBE(...)  REP_NODE(T_BOOL, OPC_SETNBE,  __VA_ARGS__)
#define REP_SETLE(...)   REP_NODE(T_BOOL, OPC_SETLE,   __VA_ARGS__)
#define REP_SETNLE(...)  REP_NODE(T_BOOL, OPC_SETNLE,  __VA_ARGS__)
#define REP_SETA(...)    REP_NODE(T_BOOL, OPC_SETA,    __VA_ARGS__)
#define REP_SETNA(...)   REP_NODE(T_BOOL, OPC_SETNA,   __VA_ARGS__)
#define REP_SETG(...)    REP_NODE(T_BOOL, OPC_SETG,    __VA_ARGS__)
#define REP_SETNG(...)   REP_NODE(T_BOOL, OPC_SETNG,   __VA_ARGS__)
#define REP_SETAE(...)   REP_NODE(T_BOOL, OPC_SETAE,   __VA_ARGS__)
#define REP_SETNAE(...)  REP_NODE(T_BOOL, OPC_SETNAE,  __VA_ARGS__)
#define REP_SETGE(...)   REP_NODE(T_BOOL, OPC_SETGE,   __VA_ARGS__)
#define REP_SETNGE(...)  REP_NODE(T_BOOL, OPC_SETNGE,  __VA_ARGS__)

#define REP_JMP(...)     REP_NODE(T_SAME, OPC_JMP,     __VA_ARGS__)

#define REP_JE(...)      REP_NODE(T_BOOL, OPC_JE,      __VA_ARGS__)
#define REP_JNE(...)     REP_NODE(T_BOOL, OPC_JNE,     __VA_ARGS__)
#define REP_JB(...)      REP_NODE(T_BOOL, OPC_JB,      __VA_ARGS__)
#define REP_JNB(...)     REP_NODE(T_BOOL, OPC_JNB,     __VA_ARGS__)
#define REP_JL(...)      REP_NODE(T_BOOL, OPC_JL,      __VA_ARGS__)
#define REP_JNL(...)     REP_NODE(T_BOOL, OPC_JNL,     __VA_ARGS__)
#define REP_JBE(...)     REP_NODE(T_BOOL, OPC_JBE,     __VA_ARGS__)
#define REP_JNBE(...)    REP_NODE(T_BOOL, OPC_JNBE,    __VA_ARGS__)
#define REP_JLE(...)     REP_NODE(T_BOOL, OPC_JLE,     __VA_ARGS__)
#define REP_JNLE(...)    REP_NODE(T_BOOL, OPC_JNLE,    __VA_ARGS__)
#define REP_JA(...)      REP_NODE(T_BOOL, OPC_JA,      __VA_ARGS__)
#define REP_JNA(...)     REP_NODE(T_BOOL, OPC_JNA,     __VA_ARGS__)
#define REP_JG(...)      REP_NODE(T_BOOL, OPC_JG,      __VA_ARGS__)
#define REP_JNG(...)     REP_NODE(T_BOOL, OPC_JNG,     __VA_ARGS__)
#define REP_JAE(...)     REP_NODE(T_BOOL, OPC_JAE,     __VA_ARGS__)
#define REP_JNAE(...)    REP_NODE(T_BOOL, OPC_JNAE,    __VA_ARGS__)
#define REP_JGE(...)     REP_NODE(T_BOOL, OPC_JGE,     __VA_ARGS__)
#define REP_JNGE(...)    REP_NODE(T_BOOL, OPC_JNGE,    __VA_ARGS__)

#define LOAD_PAT(COST, TY)                  \
    {                                       \
        .cost = COST,                       \
        .pat = LOAD_PAT_NODE(TY),           \
        .reps = { REP_MOVE_RM(-1, 0) }      \
    }

#define STORE_PAT(COST, TY)                             \
    {                                                   \
        .cost = COST,                                   \
        .pat = {                                        \
            .kind = PatNode::Inst,                      \
            .ty = TY,                                   \
            .opc = InstrOpcode::OP_STORE,               \
            .opnd = OperandKind::OK_POISON,             \
            .opnds = { PAT_SLOT_OPND, PAT_REG_OPND }    \
        },                                              \
        .reps = { REP_MOVE_MR(0, 1) }                   \
    }

#define ADD_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = ADD_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_ADD(-1, 1) }          \
    }

#define ADD_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = ADD_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_ADD(-1, 1) }                  \
    }

#define ADD_MR_PAT(COST, TY) \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = ADD_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_ADD(-1, 1) }                  \
    }

#define SUB_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = SUB_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_SUB(-1, 1) }          \
    }

#define SUB_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = SUB_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_SUB(-1, 1) }                  \
    }

#define SUB_MR_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = SUB_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_SUB(-1, 1) }                  \
    }

#define SUB_RI_PAT(COST, TY)                                        \
    {                                                               \
        .cost = COST,                                               \
        .pat = SUB_PAT_NODE(TY, PAT_REG_OPND, PAT_IMM_INT_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_SUB(-1, 1) }              \
    }

#define SUB_IR_PAT(COST, TY)                                        \
    {                                                               \
        .cost = COST,                                               \
        .pat = SUB_PAT_NODE(TY, PAT_IMM_INT_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 1), REP_SUB(-1, 0) }              \
    }

#define MUL_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = MUL_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_MUL(-1, 1) }          \
    }

#define MUL_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = MUL_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_MUL(-1, 1) }                  \
    }

#define MUL_MR_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = MUL_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 1), REP_MUL(-1, 0) }                  \
    }

#define IMUL_RR_PAT(COST, TY)                                   \
    {                                                           \
        .cost = COST,                                           \
        .pat = MUL_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_IMUL(-1, 1) }         \
    }

#define IMUL_RM_PAT(COST, TY)                                           \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = MUL_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_IMUL(-1, 1) }                 \
    }

#define IMUL_MR_PAT(COST, TY)                                           \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = MUL_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 1), REP_IMUL(-1, 0) }                 \
    }

#define AND_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = AND_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_AND(-1, 1) }          \
    }

#define AND_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = AND_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_AND(-1, 1) }                  \
    }

#define AND_MR_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = AND_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_AND(-1, 1) }                  \
    }

#define OR_RR_PAT(COST, TY)                                 \
    {                                                       \
        .cost = COST,                                       \
        .pat = OR_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND), \
        .reps = { REP_MOVE_RR(-1, 0), REP_OR(-1, 1) }       \
    }

#define OR_RM_PAT(COST, TY)                                             \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = OR_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_OR(-1, 1) }                   \
    }

#define OR_MR_PAT(COST, TY)                                             \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = OR_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_OR(-1, 1) }                   \
    }

#define XOR_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = XOR_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_XOR(-1, 1) }          \
    }

#define XOR_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = XOR_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_XOR(-1, 1) }                  \
    }

#define XOR_MR_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = XOR_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_XOR(-1, 1) }                  \
    }

#define SHL_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = SHL_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_SHL(-1, 1) }          \
    }

#define SHL_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = SHL_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_SHL(-1, 1) }                  \
    }

#define SHL_MR_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = SHL_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_SHL(-1, 1) }                  \
    }

#define SHR_RR_PAT(COST, TY)                                    \
    {                                                           \
        .cost = COST,                                           \
        .pat = SHR_PAT_NODE(TY, PAT_REG_OPND, PAT_REG_OPND),    \
        .reps = { REP_MOVE_RR(-1, 0), REP_SHR(-1, 1) }          \
    }

#define SHR_RM_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = SHR_PAT_NODE(TY, PAT_REG_OPND, LOAD_PAT_NODE(T_SAME)),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_SHR(-1, 1) }                  \
    }

#define SHR_MR_PAT(COST, TY)                                            \
    {                                                                   \
        .cost = COST,                                                   \
        .pat = SHR_PAT_NODE(TY, LOAD_PAT_NODE(T_SAME), PAT_REG_OPND),   \
        .reps = { REP_MOVE_RR(-1, 0), REP_SHR(-1, 1) }                  \
    }

#define CMP_EQ_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_EQ_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETE(-1) }                     \
    }

#define CMP_EQ_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_EQ_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETE(-1) }                             \
    }

#define CMP_EQ_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_EQ_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETE(-1) }                     \
    }

#define CMP_NE_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_NE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETNE(-1) }                    \
    }

#define CMP_NE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_NE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETNE(-1) }                            \
    }

#define CMP_NE_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_NE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETNE(-1) }                    \
    }

#define CMP_LT_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETL(-1) }                     \
    }

#define CMP_LT_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETL(-1) }                             \
    }

#define CMP_LT_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETL(-1) }                     \
    }

#define CMP_B_RR_PAT(COST, TY)                                      \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETB(-1) }                     \
    }

#define CMP_B_RM_PAT(COST, TY)                                              \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETB(-1) }                             \
    }

#define CMP_B_MR_PAT(COST, TY)                                      \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETB(-1) }                     \
    }

#define CMP_LE_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETLE(-1) }                    \
    }

#define CMP_LE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETLE(-1) }                            \
    }

#define CMP_LE_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETLE(-1) }                    \
    }

#define CMP_LE_INT_IM_PAT(COST)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_LE_PAT_NODE(PAT_IMM_INT_OPND, LOAD_PAT_NODE(T_SAME)),    \
        .reps = { REP_CMP(1, 0), REP_SETLE(-1) }                            \
    }

#define CMP_LE_INT_MI_PAT(COST)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_LE_PAT_NODE(LOAD_PAT_NODE(T_ANY_I), PAT_IMM_INT_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETLE(-1) }                            \
    }

#define CMP_BE_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETBE(-1) }                    \
    }

#define CMP_BE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETBE(-1) }                            \
    }

#define CMP_BE_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_LE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETBE(-1) }                    \
    }

#define CMP_GT_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETG(-1) }                     \
    }

#define CMP_GT_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETG(-1) }                             \
    }

#define CMP_GT_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETG(-1) }                     \
    }

#define CMP_A_RR_PAT(COST, TY)                                      \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETA(-1) }                     \
    }

#define CMP_A_RM_PAT(COST, TY)                                              \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETA(-1) }                             \
    }

#define CMP_A_MR_PAT(COST, TY)                                      \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETA(-1) }                     \
    }

#define CMP_GE_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETGE(-1) }                    \
    }

#define CMP_GE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETGE(-1) }                            \
    }

#define CMP_GE_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETGE(-1) }                    \
    }

#define CMP_AE_RR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND),   \
        .reps = { REP_CMP(0, 1), REP_SETAE(-1) }                    \
    }

#define CMP_AE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)),  \
        .reps = { REP_CMP(0, 1), REP_SETAE(-1) }                            \
    }

#define CMP_AE_MR_PAT(COST, TY)                                     \
    {                                                               \
        .cost = COST,                                               \
        .pat = CMP_GE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND),    \
        .reps = { REP_CMP(0, 1), REP_SETAE(-1) }                    \
    }


#define BRC_EQ_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_EQ_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JE(2), REP_JMP(3) }        \
    }

#define BRC_EQ_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_EQ_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JE(2), REP_JMP(3) }                    \
    }

#define BRC_EQ_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_EQ_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JE(2), REP_JMP(3) }                    \
    }

#define BRC_NE_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_NE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JNE(2), REP_JMP(3) }           \
    }

#define BRC_NE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_NE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JNE(2), REP_JMP(3) }                   \
    }

#define BRC_NE_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_NE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JNE(2), REP_JMP(3) }                   \
    }

#define BRC_LT_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JL(2), REP_JMP(3) }        \
    }

#define BRC_LT_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JL(2), REP_JMP(3) }                    \
    }

#define BRC_LT_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JL(2), REP_JMP(3) }                    \
    }

#define BRC_B_RR_PAT(COST, TY)                                  \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JB(2), REP_JMP(3) }        \
    }

#define BRC_B_RM_PAT(COST, TY)                                              \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JB(2), REP_JMP(3) }                    \
    }

#define BRC_B_MR_PAT(COST, TY)                                              \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JB(2), REP_JMP(3) }                    \
    }

#define BRC_LE_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JLE(2), REP_JMP(3) }       \
    }

#define BRC_LE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JLE(2), REP_JMP(3) }                   \
    }

#define BRC_LE_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JLE(2), REP_JMP(3) }                   \
    }

#define BRC_LE_INT_IM_PAT(COST)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(PAT_IMM_INT_OPND, LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(1, 0), REP_JLE(2), REP_JMP(3) }                   \
    }

#define BRC_LE_INT_MI_PAT(COST)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(LOAD_PAT_NODE(T_ANY_I), PAT_IMM_INT_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JLE(2), REP_JMP(3) }                   \
    }

#define BRC_BE_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JBE(2), REP_JMP(3) }       \
    }

#define BRC_BE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JBE(2), REP_JMP(3) }                   \
    }

#define BRC_BE_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_LE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JBE(2), REP_JMP(3) }                   \
    }

#define BRC_GT_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JG(2), REP_JMP(3) }        \
    }

#define BRC_GT_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JG(2), REP_JMP(3) }                    \
    }

#define BRC_GT_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JG(2), REP_JMP(3) }                    \
    }

#define BRC_A_RR_PAT(COST, TY)                                  \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JA(2), REP_JMP(3) }        \
    }

#define BRC_A_RM_PAT(COST, TY)                                              \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GT_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JA(2), REP_JMP(3) }                    \
    }

#define BRC_A_MR_PAT(COST, TY)                                              \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GT_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JA(2), REP_JMP(3) }                    \
    }

#define BRC_GE_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JGE(2), REP_JMP(3) }       \
    }

#define BRC_GE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JGE(2), REP_JMP(3) }                   \
    }

#define BRC_GE_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JGE(2), REP_JMP(3) }                   \
    }

#define BRC_AE_RR_PAT(COST, TY)                                 \
    {                                                           \
        .cost = COST,                                           \
        .pat = BRC_PAT_NODE(CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JAE(2), REP_JMP(3) }       \
    }

#define BRC_AE_RM_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GE_PAT_NODE(PAT_REG_OPND_T(TY), LOAD_PAT_NODE(T_SAME)), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JAE(2), REP_JMP(3) }                   \
    }

#define BRC_AE_MR_PAT(COST, TY)                                             \
    {                                                                       \
        .cost = COST,                                                       \
        .pat = BRC_PAT_NODE(CMP_GE_PAT_NODE(LOAD_PAT_NODE(TY), PAT_REG_OPND), PAT_BBLOCK_OPND, PAT_BBLOCK_OPND), \
        .reps = { REP_CMP(0, 1), REP_JAE(2), REP_JMP(3) }                   \
    }

#define NOT_R_PAT(COST, TY)                         \
    {                                               \
        .cost = COST,                               \
        .pat = NOT_PAT_NODE(TY, PAT_REG_OPND),      \
        .reps = { REP_MOVE_RR(-1, 0), REP_NOT(-1) } \
    }

#define BR_PAT(COST)                            \
    {                                           \
        .cost = COST,                           \
        .pat = BR_PAT_NODE(PAT_BBLOCK_OPND),    \
        .reps = { REP_JMP(0) }                  \
    }

//#define BRC_PAT(COST, TY)                                                           \
//    {                                                                               \
//        .cost = COST,                                                               \
//        .pat = BRC_PAT_NODE(TY, PAT_REG_OPND, PAT_BBLOCK_OPND, PAT_BBLOCK_OPND),    \
//        .reps = { REP_JMP(0) }                                                      \
//    }

#define RET_PAT(COST, TY)                       \
    {                                           \
        .cost = COST,                           \
        .pat = RET_PAT_NODE(TY, PAT_REG_OPND),  \
        .reps = { REP_RET(0) }                  \
    }

#define RET_INT_IMM_PAT(COST)                           \
    {                                                   \
        .cost = COST,                                   \
        .pat = RET_PAT_NODE(T_ANY_I, PAT_IMM_INT_OPND), \
        .reps = { REP_RET(0) }                          \
    }

#define CALL_PAT(COST, TY)              \
    {                                   \
        .cost = COST,                   \
        .pat = CALL_PAT_NODE(TY),       \
        .reps = { REP_CALL(T_SAME) }    \
    }

    std::vector<Pat> X64Target::load_pats()
    {
        return std::initializer_list<Pat>
        {
            // single load signed int from frame slot
            LOAD_PAT(1, T_ANY_I),
            // single load unsigned int from frame slot
            LOAD_PAT(1, T_ANY_U),
            // single store signed int to frame slot
            STORE_PAT(1, T_ANY_I),
            // single store unsigned int to frame slot
            STORE_PAT(1, T_ANY_U),
            // add 2 signed integer registers
            ADD_RR_PAT(1, T_ANY_I),
            // add 2 unsigned integer registers
            ADD_RR_PAT(1, T_ANY_U),
            // add mem with signed integer register (1)
            ADD_RM_PAT(1, T_ANY_I),
            // add mem with signed integer register (2)
            ADD_MR_PAT(1, T_ANY_I),
            // add mem with unsigned integer register (1)
            ADD_RM_PAT(1, T_ANY_U),
            // add mem with unsigned integer register (2)
            ADD_MR_PAT(1, T_ANY_U),
            // sub 2 signed integer registers
            SUB_RR_PAT(1, T_ANY_I),
            // sub 2 unsigned integer registers
            SUB_RR_PAT(1, T_ANY_U),
            // sub mem with signed integer register (1)
            SUB_RM_PAT(1, T_ANY_I),
            // sub mem with signed integer register (2)
            SUB_MR_PAT(1, T_ANY_I),
            // sub mem with unsigned integer register (1)
            SUB_RM_PAT(1, T_ANY_U),
            // sub mem with unsigned integer register (2)
            SUB_MR_PAT(1, T_ANY_U),
            SUB_RI_PAT(1, T_ANY_I),
            SUB_IR_PAT(1, T_ANY_I),
            MUL_RR_PAT(1, T_ANY_U),
            MUL_RM_PAT(1, T_ANY_U),
            MUL_MR_PAT(1, T_ANY_U),
            IMUL_RM_PAT(1, T_ANY_I),
            IMUL_MR_PAT(1, T_ANY_I),
            IMUL_RR_PAT(1, T_ANY_I),
            // TODO DIV, must be precolored
            AND_RM_PAT(1, T_ANY_I),
            AND_MR_PAT(1, T_ANY_I),
            AND_RR_PAT(1, T_ANY_I),
            OR_RM_PAT(1, T_ANY_I),
            OR_MR_PAT(1, T_ANY_I),
            OR_RR_PAT(1, T_ANY_I),
            XOR_RM_PAT(1, T_ANY_I),
            XOR_MR_PAT(1, T_ANY_I),
            XOR_RR_PAT(1, T_ANY_I),
            // TODO must be precolored if using register as RHS
            //SHL_RM_PAT(1, T_ANY_I),
            //SHL_MR_PAT(1, T_ANY_I),
            //SHL_RR_PAT(1, T_ANY_I),
            //SHR_RM_PAT(1, T_ANY_I),
            //SHR_MR_PAT(1, T_ANY_I),
            //SHR_RR_PAT(1, T_ANY_I),

            CMP_EQ_RR_PAT(1, T_ANY_I),
            CMP_EQ_RM_PAT(1, T_ANY_I),
            CMP_EQ_MR_PAT(1, T_ANY_I),
            CMP_EQ_RR_PAT(1, T_ANY_U),
            CMP_EQ_RM_PAT(1, T_ANY_U),
            CMP_EQ_MR_PAT(1, T_ANY_U),
            CMP_NE_RR_PAT(1, T_ANY_I),
            CMP_NE_RM_PAT(1, T_ANY_I),
            CMP_NE_MR_PAT(1, T_ANY_I),
            CMP_NE_RR_PAT(1, T_ANY_U),
            CMP_NE_RM_PAT(1, T_ANY_U),
            CMP_NE_MR_PAT(1, T_ANY_U),
            CMP_LT_RR_PAT(1, T_ANY_I),
            CMP_LT_RM_PAT(1, T_ANY_I),
            CMP_LT_MR_PAT(1, T_ANY_I),
            CMP_B_RR_PAT(1, T_ANY_U),
            CMP_B_RM_PAT(1, T_ANY_U),
            CMP_B_MR_PAT(1, T_ANY_U),
            CMP_LE_RR_PAT(1, T_ANY_I),
            CMP_LE_RM_PAT(1, T_ANY_I),
            CMP_LE_MR_PAT(1, T_ANY_I),
            CMP_LE_INT_IM_PAT(1),
            CMP_LE_INT_MI_PAT(1),
            CMP_BE_RR_PAT(1, T_ANY_U),
            CMP_BE_RM_PAT(1, T_ANY_U),
            CMP_BE_MR_PAT(1, T_ANY_U),
            CMP_GT_RR_PAT(1, T_ANY_I),
            CMP_GT_RM_PAT(1, T_ANY_I),
            CMP_GT_MR_PAT(1, T_ANY_I),
            CMP_A_RR_PAT(1, T_ANY_U),
            CMP_A_RM_PAT(1, T_ANY_U),
            CMP_A_MR_PAT(1, T_ANY_U),
            CMP_GE_RR_PAT(1, T_ANY_I),
            CMP_GE_RM_PAT(1, T_ANY_I),
            CMP_GE_MR_PAT(1, T_ANY_I),
            CMP_AE_RR_PAT(1, T_ANY_U),
            CMP_AE_RM_PAT(1, T_ANY_U),
            CMP_AE_MR_PAT(1, T_ANY_U),

            BR_PAT(1),
            // TODO do after immediates
            //BRC_PAT(1, T_STATIC_ADDRESS),

            BRC_EQ_RR_PAT(1, T_ANY_I),
            BRC_EQ_RM_PAT(1, T_ANY_I),
            BRC_EQ_MR_PAT(1, T_ANY_I),
            BRC_EQ_RR_PAT(1, T_ANY_U),
            BRC_EQ_RM_PAT(1, T_ANY_U),
            BRC_EQ_MR_PAT(1, T_ANY_U),
            BRC_NE_RR_PAT(1, T_ANY_I),
            BRC_NE_RM_PAT(1, T_ANY_I),
            BRC_NE_MR_PAT(1, T_ANY_I),
            BRC_NE_RR_PAT(1, T_ANY_U),
            BRC_NE_RM_PAT(1, T_ANY_U),
            BRC_NE_MR_PAT(1, T_ANY_U),
            BRC_LT_RR_PAT(1, T_ANY_I),
            BRC_LT_RM_PAT(1, T_ANY_I),
            BRC_LT_MR_PAT(1, T_ANY_I),
            BRC_B_RR_PAT(1, T_ANY_U),
            BRC_B_RM_PAT(1, T_ANY_U),
            BRC_B_MR_PAT(1, T_ANY_U),
            BRC_LE_RR_PAT(1, T_ANY_I),
            BRC_LE_RM_PAT(1, T_ANY_I),
            BRC_LE_MR_PAT(1, T_ANY_I),
            BRC_LE_INT_IM_PAT(1),
            BRC_LE_INT_MI_PAT(1),
            BRC_BE_RR_PAT(1, T_ANY_U),
            BRC_BE_RM_PAT(1, T_ANY_U),
            BRC_BE_MR_PAT(1, T_ANY_U),
            BRC_GT_RR_PAT(1, T_ANY_I),
            BRC_GT_RM_PAT(1, T_ANY_I),
            BRC_GT_MR_PAT(1, T_ANY_I),
            BRC_A_RR_PAT(1, T_ANY_U),
            BRC_A_RM_PAT(1, T_ANY_U),
            BRC_A_MR_PAT(1, T_ANY_U),
            BRC_GE_RR_PAT(1, T_ANY_I),
            BRC_GE_RM_PAT(1, T_ANY_I),
            BRC_GE_MR_PAT(1, T_ANY_I),
            BRC_AE_RR_PAT(1, T_ANY_U),
            BRC_AE_RM_PAT(1, T_ANY_U),
            BRC_AE_MR_PAT(1, T_ANY_U),

            // return an integer register
            RET_PAT(1, T_ANY_I),
            RET_INT_IMM_PAT(1),
            // return an unsigned integer register
            RET_PAT(1, T_ANY_U),

            CALL_PAT(1, T_ANY_I)
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
        switch (inst.opc)
        {
            default:
                out << OPCS.at(inst.opc);
                break;

            case OPC_MOVE_RR:
                out << "\tmov_rr\t";
                break;

            case OPC_MOVE_MR:
                out << "\tmov_mr\t";
                break;

            case OPC_MOVE_RM:
                out << "\tmov_rm\t";
                break;

            case OPC_MOVE_RI:
                out << "\tmov_ri\t";
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

            if (machine_insts.back().opc == OPC_RET && !machine_insts.back().opnds.empty())
            {
                auto out_ret = machine_insts.back().opnds.front();
                machine_insts.pop_back();

                RegisterID reg = RAX;
                reg.size = out_ret.ty.size;

                auto opc = out_ret.kind == MachineOperand::Register
                        ? OPC_MOVE_RR
                        : OPC_MOVE_RI;

                auto& mov = machine_insts.emplace_back(opc);
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
                exit_id = proc.add_bblock("exit");

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

                exit.machine_insts().insert(stackdown_insertpoint, std::move(inst));
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
            exit.machine_insts().insert(stackdown_insertpoint, std::move(restore_rsp));

            // restore rsp
            MachineInstruction restore_rbp;
            restore_rbp.opc = OPC_POP;
            restore_rbp.size = RBP.size;
            restore_rbp.opnds.push_back({
                .kind = MachineOperand::Register,
                .val = std::bit_cast<std::uint64_t>(RBP)
            });
            exit.machine_insts().insert(stackdown_insertpoint, std::move(restore_rbp));
        }

        // for each function call add stackup & stackdown
        for (auto& bblock: proc.bblocks())
        {
            auto& m_insts = bblock.machine_insts();
            auto inst_it = m_insts.begin();

            while (inst_it != m_insts.end())
            {
                auto& inst = *inst_it;

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

                // add stack maintenance to function calls
                if (inst.opc == OPC_CALL)
                {
                    auto next_it = inst_it;
                    ++next_it;

                    // compute live regs at the instructions time
                    std::set<RegisterID> live_regs;
                    for (auto& [reg, ty]: bblock.live_outs())
                    {
                        live_regs.insert(reg);
                    }

                    auto rev_it = --m_insts.end();
                    while (rev_it != inst_it && rev_it != m_insts.begin())
                    {
                        for (auto& opnd: rev_it->opnds)
                        {
                            if (opnd.kind != MachineOperand::Register)
                            {
                                continue;
                            }

                            auto reg = std::bit_cast<RegisterID>(opnd.val);

                            if (opnd.is_def)
                            {
                                live_regs.erase(reg);
                            }
                            else
                            {
                                live_regs.insert(reg);
                            }
                        }

                        --rev_it;
                    }

                    // preserve caller saved clobbers
                    for (auto reg: live_regs)
                    {
                        if (!CALLER_SAVED_REGS.contains(reg))
                        {
                            continue;
                        }

                        // push register
                        MachineInstruction inst;
                        inst.opc = OPC_PUSH;
                        inst.size = reg.size;

                        MachineOperand opnd;
                        opnd.kind = MachineOperand::Register;
                        opnd.val = std::bit_cast<std::uint64_t>(reg);
                        inst.opnds.push_back(opnd);

                        m_insts.insert(inst_it, inst);

                        // pop register
                        inst.opnds.clear();
                        inst.opc = OPC_POP;
                        inst.size = reg.size;

                        opnd.kind = MachineOperand::Register;
                        opnd.val = std::bit_cast<std::uint64_t>(reg);
                        inst.opnds.push_back(opnd);

                        m_insts.insert(next_it, std::move(inst));
                    }

                    // put everything on the correct registers
                    for (auto& opnd: inst.opnds)
                    {
                        int ret_idx = 0, arg_idx = 0;

                            std::cout << "!!!OPND!!! " << std::endl;

                        if (opnd.is_def)
                        {
                            auto ret_reg = RETURN_REGISTERS[arg_idx++];

                            std::cout << "ret!!! " << ret_reg.val << std::endl;

                            if (std::bit_cast<RegisterID>(opnd.val).val != ret_reg.val)
                            {
                                MachineInstruction inst;
                                inst.opc = OPC_MOVE_RR;
                                inst.size = opnd.ty.size;

                                inst.opnds.push_back(opnd);

                                MachineOperand reg_opnd;
                                reg_opnd.kind = MachineOperand::Register;
                                reg_opnd.val = std::bit_cast<std::uint64_t>(ret_reg);
                                inst.opnds.push_back(reg_opnd);

                                m_insts.insert(inst_it, inst);
                            }
                        }
                        else
                        {
                            auto arg_reg = ARG_REGISTERS[arg_idx++];

                            std::cout << "arg!!! " << arg_reg.val << std::endl;

                            auto move_to_arg_reg = [&](auto opc)
                            {
                                MachineInstruction inst;
                                inst.opc = opc;
                                inst.size = opnd.ty.size;

                                arg_reg.size = inst.size;

                                MachineOperand reg_opnd;
                                reg_opnd.kind = MachineOperand::Register;
                                reg_opnd.val = std::bit_cast<std::uint64_t>(arg_reg);
                                inst.opnds.push_back(reg_opnd);

                                inst.opnds.push_back(opnd);
                                m_insts.insert(inst_it, inst);
                            };

                            if (opnd.kind == MachineOperand::Imm)
                            {
                                move_to_arg_reg(OPC_MOVE_RI);
                            }
                            else if (opnd.kind == MachineOperand::MemAddr)
                            {
                                move_to_arg_reg(OPC_MOVE_RM);
                            }
                            else if (opnd.kind == MachineOperand::Register && std::bit_cast<RegisterID>(opnd.val).val != arg_reg.val)
                            {
                                move_to_arg_reg(OPC_MOVE_RR);
                            }

                        }
                    }

                    inst.opnds.clear();
                }

                ++inst_it;
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

            dump_proc(proc, std::cout);

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

                    if (inst.id != "")
                    {
                        out << inst.id << '\t';
                    }

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
                                out.flush();
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
