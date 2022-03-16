#pragma once

#include <memory>
#include <iostream>
#include <vector>

namespace ucb
{
    class CompileUnit;

    enum TypeID : int
    {
        T_ERROR = -10,

        T_VOID = 0,
        T_STATIC_ADDRESS,

        T_BOOL,
        T_U8,
        T_U16,
        T_U32,
        T_U64,
        T_I8,
        T_I16,
        T_I32,
        T_I64,
        T_F32,
        T_F64,
    };

    constexpr TypeID COMP_TY_START = static_cast<TypeID>(20);

    class CompositeType
    {
    public:
        enum CompositeTyKind
        {
            CTK_PTR,
            CTK_ARR
        };

        CompositeType(CompileUnit *parent, CompositeTyKind kind, int size, TypeID sub):
            _parent(parent),
            _kind{kind},
            _size{size},
            _sub{sub}
        {
        }

        bool operator == (const CompositeType& other) const
        {
            return !operator!=(other);
        }

        bool operator != (const CompositeType& other) const
        {
            return _parent != other._parent
                || _sub != other._sub
                || _kind != other._kind
                || _size != other._size;
        }

        CompositeTyKind kind() const { return _kind; }
        int size() const { return _size; }
        TypeID sub() const { return _sub; }

    private:
        CompileUnit *_parent;
        CompositeTyKind _kind;
        int _size;
        TypeID _sub;
    };
}
