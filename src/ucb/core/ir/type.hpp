#pragma once

#include <iostream>
#include <vector>

namespace ucb
{
    class Type;
    class IntegralTy;
    class PtrTy;
    class ArrayTy;
    class CompileUnit;

    class Type
    {
    public:
        enum TypeKind : char
        {
            TK_INT,
            TK_PTR,
            TK_ARRAY
        };

        virtual ~Type() = default;

        bool operator == (const Type& other) const;
        bool operator != (const Type& other) const;

        TypeKind kind() const { return _kind; }

        IntegralTy* as_int_ty();
        const IntegralTy* as_int_ty() const;

        PtrTy* as_ptr_ty();
        const PtrTy* as_ptr_ty() const;

        ArrayTy* as_array_ty();
        const ArrayTy* as_array_ty() const;

        friend IntegralTy;
        friend PtrTy;
        friend ArrayTy;

    private:
        Type(CompileUnit *parent):
            _parent{parent}
        {
        }

        CompileUnit *_parent;
        TypeKind _kind;
        std::vector<Type*> _owned_tys;
    };

    class IntegralTy : public Type
    {
    public:
        enum IntegralKind : char
        {
            IK_SIGNED,
            IK_UNSIGNED,
            IK_FLOAT
        };

        IntegralTy(CompileUnit *parent, IntegralKind ikind, char size_bits):
            Type(parent),
            _ikind{ikind},
            _size_bits{size_bits}
        {
        }

        bool operator == (const IntegralTy& other) const
        {
            return _parent == other._parent
                && _ikind == other._ikind
                && _size_bits == other._size_bits;
        }

        bool operator != (const IntegralTy& other) const
        {
            return !(*this == other);
        }

        IntegralKind ikind() const { return _ikind; }
        char size_bits() const { return _size_bits; }

    private:
        IntegralKind _ikind;
        char _size_bits;
    };

    class PtrTy : public Type
    {
    public:
        PtrTy(CompileUnit *parent, Type *base):
            Type(parent)
        {
            _owned_tys.push_back(base);
        }

        bool operator == (const PtrTy& other) const
        {
            if (_parent != other._parent ||
                _owned_tys.size() != 1 ||
                other._owned_tys.size() != 1)
            {
                return false;
            }

            return *(_owned_tys.front()) == *(other._owned_tys.front());
        }

        bool operator != (const PtrTy& other) const
        {
            return !(*this == other);
        }
    };

    class ArrayTy : public Type
    {
    public:
        ArrayTy(CompileUnit *parent, Type *base, int size):
            Type(parent),
            _size{size}
        {
            _owned_tys.push_back(base);
        }

        bool operator == (const ArrayTy& other) const
        {
            if (_parent != other._parent ||
                _size != other._size ||
                _owned_tys.size() != 1 ||
                other._owned_tys.size() != 1)
            {
                return false;
            }

            return *(_owned_tys.front()) == *(other._owned_tys.front());
        }

        bool operator != (const ArrayTy& other) const
        {
            return !(*this == other);
        }

        int size() const { return _size; }

    private:
        int _size;
    };

    bool Type::operator == (const Type& other) const
    {
        if (_parent != other._parent || _kind != other._kind)
        {
            return false;
        }

        switch (_kind)
        {
        case TypeKind::TK_INT:
            return *as_int_ty() == *other.as_int_ty();

        case TypeKind::TK_PTR:
            return *as_ptr_ty() == *other.as_ptr_ty();

        case TypeKind::TK_ARRAY:
            return *as_array_ty() == *other.as_array_ty();
        }
    }

    bool Type::operator != (const Type& other) const
    {
        return !(*this == other);
    }
    
    IntegralTy* Type::as_int_ty()
    {
        if (_kind != TypeKind::TK_INT)
        {
            std::cerr << "tried to convert an invalid type to integral" << std::endl;
            abort();
        }

        return dynamic_cast<IntegralTy*>(this);
    }

    const IntegralTy* Type::as_int_ty() const
    {
        if (_kind != TypeKind::TK_INT)
        {
            std::cerr << "tried to convert an invalid type to integral" << std::endl;
            abort();
        }

        return dynamic_cast<const IntegralTy*>(this);
    }

    PtrTy* Type::as_ptr_ty()
    {
        if (_kind != TypeKind::TK_PTR)
        {
            std::cerr << "tried to convert an invalid type to ptr" << std::endl;
            abort();
        }

        return dynamic_cast<PtrTy*>(this);
    }

    const PtrTy* Type::as_ptr_ty() const
    {
        if (_kind != TypeKind::TK_PTR)
        {
            std::cerr << "tried to convert an invalid type to ptr" << std::endl;
            abort();
        }

        return dynamic_cast<const PtrTy*>(this);
    }

    ArrayTy* Type::as_array_ty()
    {
        if (_kind != TypeKind::TK_PTR)
        {
            std::cerr << "tried to convert an invalid type to array" << std::endl;
            abort();
        }

        return dynamic_cast<ArrayTy*>(this);
    }

    const ArrayTy* Type::as_array_ty() const
    {
        if (_kind != TypeKind::TK_PTR)
        {
            std::cerr << "tried to convert an invalid type to array" << std::endl;
            abort();
        }

        return dynamic_cast<const ArrayTy*>(this);
    }
}
