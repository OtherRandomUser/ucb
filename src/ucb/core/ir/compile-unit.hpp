#pragma once

#include <vector>

#include <ucb/core/ir/type.hpp>

namespace ucb
{
    class CompileUnit
    {
    public:
        CompileUnit();

        template<typename... ARGS>
        IntegralTy *get_int_ty(ARGS&&... args)
        {
            IntegralTy ty(this, std::forward(args)...);
            auto it = std::find(_integral_tys.begin(), _integral_tys.end(), ty);

            if (it == _integral_tys.end())
            {
                _integral_tys.push_back(std::move(ty));
                return &_integral_tys.back();
            }
            else
            {
                return &(*it);
            }
        };

        template<typename... ARGS>
        PtrTy *get_ptr_ty(ARGS&&... args)
        {
            PtrTy ty(this, std::forward(args)...);
            auto it = std::find(_ptr_tys.begin(), _ptr_tys.end(), ty);

            if (it == _ptr_tys.end())
            {
                _ptr_tys.push_back(std::move(ty));
                return &_ptr_tys.back();
            }
            else
            {
                return &(*it);
            }
        };

        template<typename... ARGS>
        ArrayTy *get_array_ty(ARGS&&... args)
        {
            ArrayTy ty(this, std::forward(args)...);
            auto it = std::find(_array_tys.begin(), _array_tys.end(), ty);

            if (it == _array_tys.end())
            {
                _array_tys.push_back(std::move(ty));
                return &_array_tys.back();
            }
            else
            {
                return &(*it);
            }
        };

    private:
        std::vector<IntegralTy> _integral_tys;
        std::vector<PtrTy> _ptr_tys;
        std::vector<ArrayTy> _array_tys;
    };
}
