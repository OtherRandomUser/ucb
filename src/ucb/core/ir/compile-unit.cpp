#include <ucb/core/ir/compile-unit.hpp>

#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    std::shared_ptr<Procedure> CompileUnit::add_procedure(ProcSignature sig, std::string id)
    {
        if (get_procedure(id) != nullptr)
        {
            std::cerr << "procedure \"" << id << "already exists\n";
            abort();
        }

        _procs.push_back(std::make_shared<Procedure>(this, std::move(id), std::move(sig)));
        return _procs.back();
    }


    std::shared_ptr<Procedure> CompileUnit::get_procedure(const std::string& id)
    {
        auto it = std::find_if(_procs.begin(), _procs.end(), [&](auto& p)
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

    void CompileUnit::dump(std::ostream& out)
    {
        auto junc = "";

        for (auto& proc: _procs)
        {
            out << junc;
            junc = "\n";
            proc->dump(out);
        }
    }

    void CompileUnit::dump_ty(std::ostream& out, TypeID ty)
    {
        if (ty >= COMP_TY_START)
        {
            auto it = std::find_if(_comp_tys.begin(), _comp_tys.end(), [&](auto p)
            {
                return p.first == ty;
            });

            if (it == _comp_tys.end())
            {
                std::cerr << "type not found\n";
                abort();
            }

            auto cty = it->second;

            switch (cty.kind())
            {
                case CompositeType::CompositeTyKind::CTK_PTR:
                    out << '^';
                    dump_ty(out, cty.sub());
                    break;

                case CompositeType::CompositeTyKind::CTK_ARR:
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
                    out << "addr";
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
