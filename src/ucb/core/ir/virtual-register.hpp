#pragma once

#include <string>
#include <vector>

#include <ucb/core/ir/type.hpp>

namespace ucb
{
    class Procedure;
    class Instruction;
/*
    struct RegisterID
    {
        std::uint64_t val : 54;
        std::uint64_t size : 10;
    };

    constexpr RegisterID NO_REG = { 0, 0 };
    constexpr std::uint64_t PREG_START = 1;
    constexpr std::uint64_t VREG_START = 100;
*/
    using RegisterID = unsigned int;

    constexpr RegisterID VREG_START = 100;
    constexpr RegisterID NO_REG = 0;

    class VirtualRegister
    {
    public:
        friend Procedure;
        friend Instruction;

        VirtualRegister(Procedure *parent, std::string id, TypeID ty):
            _parent{parent},
            _id{std::move(id)},
            _ty{ty}
        {
        }

        Procedure* parent() { return _parent; }
        const Procedure* parent() const { return _parent; }
        const std::string& id() const { return _id; }
        const TypeID ty() const { return _ty; }

    private:
        Procedure *_parent;
        std::string _id;
        TypeID _ty;

        std::vector<Instruction*> _uses;
        std::vector<Instruction*> _defs;
    };
}
