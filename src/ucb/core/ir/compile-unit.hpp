#pragma once

#include <map>
#include <ostream>
#include <vector>

#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/ir/type.hpp>

namespace ucb
{
    class CompileUnit
    {
    public:
        CompileUnit();

        std::shared_ptr<Procedure> add_procedure(ProcSignature sig, std::string id);
        std::shared_ptr<Procedure> get_procedure(const std::string& id);

        TypeID get_ptr_ty(TypeID sub)
        {
            CompositeType ty(
                this,
                CompositeType::CompositeTyKind::CTK_PTR,
                64, // shouldnt be hard coded, i know
                sub
            );

            auto it = std::find(_comp_tys.begin(), _comp_tys.end(), [&](auto p)
            {
                return p.second == ty;
            });

            if (it == _comp_tys.end())
            {
                auto res = static_cast<TypeID>(_next_ty_id++);
                _comp_tys.emplace_back(res, ty);
                return res;
            }
            else
            {
                return it->first;
            }
        };

        TypeID get_arr_ty(TypeID sub, int size)
        {
            CompositeType ty(
                this,
                CompositeType::CompositeTyKind::CTK_ARR,
                size,
                sub
            );

            auto it = std::find(_comp_tys.begin(), _comp_tys.end(), [&](auto p)
            {
                return p.second == ty;
            });

            if (it == _comp_tys.end())
            {
                auto res = static_cast<TypeID>(_next_ty_id++);
                _comp_tys.emplace_back(res, ty);
                return res;
            }
            else
            {
                return it->first;
            }
        };

        void dump(std::ostream& out) const;
        void dump_ty(std::ostream& out, TypeID ty) const;

    private:
        int _next_ty_id{COMP_TY_START};
        std::vector<std::pair<TypeID, CompositeType>> _comp_tys;
        std::vector<std::shared_ptr<Procedure>> _procs;
    };
}
