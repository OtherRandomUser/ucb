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
        if (ty.val >= COMP_TY_START)
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
            switch (ty.val)
            {
                case T_ERROR.val:
                    out << "ERROR";
                    break;

                case T_VOID.val:
                    out << "void";
                    break;

                case T_STATIC_ADDRESS.val:
                    out << "addr";
                    break;

                case T_BOOL.val:
                    out << "bool";
                    break;

                case T_U8.val:
                    out << "u" << ty.size;
                    break;

                case T_I8.val:
                    out << "i" << ty.size;
                    break;

                case T_F32.val:
                    out << "f" << ty.size;
                    break;

                default:
                    assert(false && "unreachable");
            }
        }
    }
}
