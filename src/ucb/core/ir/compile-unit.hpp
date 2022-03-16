#pragma once

#include <map>
#include <vector>

#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/ir/type.hpp>

namespace ucb
{
    class CompileUnit
    {
    public:
        CompileUnit();

        // Procedure* add_procedure(signature, std::string identifier);
        // Procedure* get_procedure(std::string identifier);

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

    private:
        int _next_ty_id{COMP_TY_START};
        std::vector<std::pair<TypeID, CompositeType>> _comp_tys;
        std::vector<std::shared_ptr<Procedure>> _procs;
    };
}
