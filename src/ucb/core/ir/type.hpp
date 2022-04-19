#pragma once

#include <memory>
#include <iostream>
#include <vector>

namespace ucb
{
    class CompileUnit;

    // TODO find a better place for this
    using MachineOpc = unsigned int;
    constexpr MachineOpc MOP_NONE = 0;

    struct TypeID
    {
        std::int64_t val : 54;
        std::uint64_t size : 10;

        bool operator != (const TypeID& other) const = default;
        bool operator == (const TypeID& other) const = default;
    };

    constexpr TypeID T_ERROR = { -10, 0 };

    constexpr TypeID T_VOID = { 0, 64 };
    constexpr TypeID T_STATIC_ADDRESS = { 1, 64 };

    constexpr TypeID T_BOOL = { 2, 8 };

    constexpr TypeID T_U8 = { 3, 8 };
    constexpr TypeID T_U16 = { 3, 16 };
    constexpr TypeID T_U32 = { 3, 32 };
    constexpr TypeID T_U64 = { 3, 64 };

    constexpr TypeID T_I8 = { 4, 8 };
    constexpr TypeID T_I16 = { 4, 16 };
    constexpr TypeID T_I32 = { 4, 32 };
    constexpr TypeID T_I64 = { 4, 64 };

    constexpr TypeID T_F32 = { 5, 32 };
    constexpr TypeID T_F64 = { 5, 64 };

    constexpr std::int64_t COMP_TY_START = 10;

/*
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
*/
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

    constexpr bool ty_is_signed_int(TypeID ty)
    {
        return ty == T_I8
            || ty == T_I16
            || ty == T_I32
            || ty == T_I64;
    }

    constexpr bool ty_is_unsigned_int(TypeID ty)
    {
        return ty == T_U8
            || ty == T_U16
            || ty == T_U32
            || ty == T_U64;
    }

    constexpr bool ty_is_float(TypeID ty)
    {
        return ty == T_F32
            || ty == T_F64;
    }

    class ProcSignature
    {
    public:
        ProcSignature(TypeID ty):
            _ty{ty}
        {
        }

        TypeID ret() const { return _ty; };
        std::vector<std::pair<std::string, TypeID>>& args() { return _args; }
        const std::vector<std::pair<std::string, TypeID>>& args() const { return _args; }

    private:
        TypeID _ty;
        std::vector<std::pair<std::string, TypeID>> _args;
    };
}
