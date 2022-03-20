#pragma once

#include <memory>
#include <vector>

#include <ucb/core/ir/basic-block.hpp>
#include <ucb/core/ir/ilist.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
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

    class Procedure : public IList<BasicBlock>
    {
    public:
        Procedure(CompileUnit *parent, std::string id, ProcSignature signature):
            _parent{parent},
            _id(std::move(id)),
            _signature(std::move(signature))
        {
            for (auto& arg: _signature.args())
            {
                _params.emplace_back(this, arg.first, arg.second);
            }
        }

        const std::string& id() const { return _id; }
        const ProcSignature& signature() { return _signature; }
        std::vector<VirtualRegister>& params() { return _params; }
        std::vector<VirtualRegister>& frame() { return _frame; }
        std::vector<VirtualRegister>& regs() { return _regs; }

        BasicBlock& entry()
        {
            assert(_bblocks.size());
            return *_bblocks.begin();
        }

        BasicBlock* find_bblock(const std::string& id);
        BasicBlock* add_bblock(std::string id);
        Operand* operand_from_bblock(const std::string& id);
        VirtualRegister* find_vreg(const std::string& id);
        VirtualRegister& add_frame_slot(std::string id, TypeID ty);
        VirtualRegister& add_vreg(std::string id, TypeID ty);
        Operand* operand_from_vreg(const std::string& id, bool is_def);

    private:
        CompileUnit *_parent;

        ProcSignature _signature;
        std::string _id;

        std::vector<VirtualRegister> _params;
        std::vector<VirtualRegister> _frame;
        std::vector<VirtualRegister> _regs;

        std::vector<BasicBlock> _bblocks;
    };
}
