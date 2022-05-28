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
            out << "NONE ";
            dump_inst();
            break;

        case DagDefKind::DDK_REG:
            context.dump_ty(out, _ty);
            out << " " << _reg << "";
            dump_inst();
            break;

        case DagDefKind::DDK_MEM:
            context.dump_ty(out, _ty);
            out << " mem slot #" << _mem_id << " ";
            dump_inst();
            break;

        case DagDefKind::DDK_IMM:
            // TODO
            context.dump_ty(out, _ty);
            out << " imm " << _imm_val;
            dump_inst();
            break;

        case DagDefKind::DDK_ADDR:
            context.dump_ty(out, _ty);
            out << " #" << _bblock_idx << " ";
            dump_inst();
            break;

        case DagDefKind::DDK_ENTRY:
            assert(false && "unreachable");

        case DagDefKind::DDK_EXIT:
            out << "EXIT ";
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

    void Dag::dump(std::ostream& out, CompileUnit& context)
    {
        for (auto& n: _root_nodes)
        {
            n->dump(out, context);
        }
    }
}
