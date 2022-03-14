#pragma once

#include <ucb/core/ir/dilist.hpp>
#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/procedure.hpp>

namespace ucb
{
    class BasicBlock : public DIList<Instruction>
    {
    public:
        BasicBlock(Procedure *parent, std::string id):
            _parent{parent},
            _id(std::move(id))
        {
        }

        const std::string& id() const { return _id; }

    private:
        Procedure *_parent;
        std::string _id;
    };
}
