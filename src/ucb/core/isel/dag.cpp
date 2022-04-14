#include <ucb/core/isel/dag.hpp>

#include <cassert>

namespace ucb
{
    void DagNode::dump(std::ostream& out, CompileUnit& context)
    {
        dump(out, context, "");
    }

    void DagNode::dump(std::ostream& out, CompileUnit& context, const std::string& pre)
    {
        out << pre;

        auto dump_inst = [&]()
        {
            switch (_opc)
            {
            case InstrOpcode::OP_NONE: out << " = NONE\n"; return;
            case InstrOpcode::OP_ADD: out << " = add\n"; return;
            case InstrOpcode::OP_SUB: out << " = sub\n"; return;
            case InstrOpcode::OP_MUL: out << " = mul\n"; return;
            case InstrOpcode::OP_DIV: out << " = div\n"; return;
            case InstrOpcode::OP_REM: out << " = rem\n"; return;
            case InstrOpcode::OP_NOT: out << " = not\n"; return;
            case InstrOpcode::OP_AND: out << " = and\n"; return;
            case InstrOpcode::OP_OR: out << " = or\n"; return;
            case InstrOpcode::OP_XOR: out << " = xor\n"; return;
            case InstrOpcode::OP_SHL: out << " = shl\n"; return;
            case InstrOpcode::OP_SHR: out << " = shr\n"; return;
            case InstrOpcode::OP_CAST: out << " = cast\n"; return;
            case InstrOpcode::OP_CMP: out << " = cmp " << _id << "\n"; return;
            case InstrOpcode::OP_BR: out << " = br\n"; return;
            case InstrOpcode::OP_BRC: out << " = brc\n"; return;
            case InstrOpcode::OP_ALLOC: out << "alloc\n"; return;
            case InstrOpcode::OP_LOAD: out << " = load\n"; return;
            case InstrOpcode::OP_STORE: out << "store\n"; return;
            case InstrOpcode::OP_CP: out << " = cp\n"; return;
            case InstrOpcode::OP_CALL: out << " = call " << _id << "\n"; return;
            case InstrOpcode::OP_RET: out << "ret\n"; return;
            }
        };

        switch (_kind)
        {
        case DagDefKind::DDK_NONE:
            assert(false && "unreachable");

        case DagDefKind::DDK_REG:
            context.dump_ty(out, _ty);
            out << " " << _reg << "";
            dump_inst();
            break;

        case DagDefKind::DDK_MEM:
            // TODO mem
            dump_inst();
            break;

        case DagDefKind::DDK_IMM:
            // TODO
            break;

        case DagDefKind::DDK_ADDR:
            // TODO
            break;

        case DagDefKind::DDK_ENTRY:
            assert(false && "unreachable");

        case DagDefKind::DDK_EXIT:
            dump_inst();
            break;
        }


        auto s = pre + "\t";

        for (auto& n: _args)
        {
            n->dump(out, context, s);
        }
    }

    std::shared_ptr<DagNode> Dag::get_register(RegisterID id, TypeID ty)
    {
        // search from last to first in order to find the last def
        for (auto it = _all_nodes.rbegin(); it != _all_nodes.rend(); it++)
        {
            auto n = *it;

            if (n->reg() == id)
            {
                assert(n->ty() == ty && "register found but type does not match");
                return n;
            }
        }

        std::cerr << "register not found\n";
        abort();
    }
/*
    std::shared_ptr<DagNode> Dag::get_int_imm(long int val, TypeID ty)
    {
        auto n = std::make_shared<DagNode>(-1, InstrOpcode::OP_NONE, DagDefKind::DDK_IMM, ty);
        n->_imm_val = std::bit_cast<std::uint64_t>(val);
        return n;
    }

    std::shared_ptr<DagNode> Dag::get_uint_imm(unsigned long val, TypeID ty)
    {
        auto n = std::make_shared<DagNode>(-1, InstrOpcode::OP_NONE, DagDefKind::DDK_IMM, ty);
        n->_imm_val = std::bit_cast<std::uint64_t>(val);
        return n;
    }


    std::shared_ptr<DagNode> get_float_imm(double val, TypeID ty);
    */

    void Dag::dump(std::ostream& out, CompileUnit& context)
    {
        for (auto& n: _root_nodes)
        {
            n->dump(out, context);
        }
    }
}
