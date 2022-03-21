#include <ucb/core/ir/compile-unit.hpp>

namespace ucb
{
    std::shared_ptr<Procedure> CompileUnit::add_procedure(ProcSignature sig, std::string id)
    {
        if (get_procedure(id) != nullptr)
        {
            std::cerr << "procedure \"" << id << "already exists\n";
            abort();
        }

        auto proc = std::make_shared<Procedure>(this, std::move(id), std::move(sig));
        _procs.push_back(proc);
        return proc;
    }


    std::shared_ptr<Procedure> CompileUnit::get_procedure(const std::string& id)
    {
        auto it = std::find_if(_procs.begin(), _procs.end(), [&](auto p)
        {
            return p->id() == id;
        });

        if (it != _procs.end())
        {
            return *it;
        }
        else
        {
            return nullptr;
        }
    }

    void CompileUnit::dump(std::ostream& out) const
    {
        auto junc = '';

        for (auto& proc: _procs)
        {
            out << junc;
            junc = '\n';
            proc->dump(out);
        }
    }

    void CompileUnit::dump_ty(std::ostream& out, TypeID ty) const
    {
        if (ty >= COMP_TY_START)
        {
            auto it = std::find(_comp_tys.begin(), _comp_tys.end(), [&](auto p)
            {
                return p.second == ty;
            });

            if (it == _comp_tys.end())
            {
                std::cerr << "type not found\n";
                abort();
            }

            auto cty = it->second;

            switch (cty.kind())
            {
                case CompositeTyKind::CTK_PTR:
                    out << '^';
                    dump_ty(out, cty.sub());
                    break;

                case CompositeTyKind::CTK_ARR:
                    out << "array " << cty.size() << " of ";
                    dump_ty(out, cty.sub());
                    break;

                default:
                    assert(false && "unreachable");
            }
        }
        else
        {
            switch (ty)
            {
                case TypeID::T_ERROR:
                    out << "ERROR";
                    break;

                case TypeID::T_VOID:
                    out << "void";
                    break;

                case TypeID::T_STATIC_ADDRESS:
                    out << "";
                    break;

                case TypeID::T_BOOL:
                    out << "bool";
                    break;

                case TypeID::T_U8:
                    out << "u8";
                    break;

                case TypeID::T_U16:
                    out << "u16";
                    break;

                case TypeID::T_U32:
                    out << "u32";
                    break;

                case TypeID::T_U64:
                    out << "u64";
                    break;

                case TypeID::T_I8:
                    out << "i8";
                    break;

                case TypeID::T_I16:
                    out << "i16";
                    break;

                case TypeID::T_I32:
                    out << "i32";
                    break;

                case TypeID::T_I64:
                    out << "i64";
                    break;

                case TypeID::T_F32:
                    out << "f32";
                    break;

                case TypeID::T_F64:
                    out << "f64";
                    break;

                default:
                    assert(false && "unreachable");
            }
        }
    }
}
