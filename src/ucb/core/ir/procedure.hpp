#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include <ucb/core/ir/compile-unit.hpp>
#include <ucb/core/ir/basic-block.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
    class Procedure
    {
    public:
        Procedure(CompileUnit *parent, std::string id, ProcSignature signature):
            _parent{parent},
            _id(std::move(id)),
            _signature(std::move(signature))
        {
            _bblocks.reserve(100);
            for (auto& arg: _signature.args())
            {
                _params.emplace_back(_next_vreg++, VirtualRegister{this, arg.first, arg.second});
            }
        }

        const std::string& id() const { return _id; }
        const ProcSignature& signature() { return _signature; }

        using RegSlot = std::pair<RegisterID, VirtualRegister>;

        std::vector<RegSlot>& params() { return _params; }
        std::vector<RegSlot>& frame() { return _frame; }
        std::vector<RegSlot>& regs() { return _regs; }

        BasicBlock& entry()
        {
            assert(_bblocks.size());
            return *_bblocks.begin();
        }

        const std::vector<BasicBlock>& bblocks() const
        {
            return _bblocks;
        }

        int find_bblock(const std::string& id);
        BasicBlock* get_bblock(int idx);
        int add_bblock(std::string id);
        Operand operand_from_bblock(const std::string& id);

        RegisterID find_vreg(const std::string& id);
        const VirtualRegister* get_register(RegisterID id) const;
        RegisterID add_frame_slot(std::string id, TypeID ty);
        RegisterID add_vreg(std::string id, TypeID ty);
        Operand operand_from_vreg(const std::string& id, bool is_def);

        void dump(std::ostream& out);
        void dump_ty(std::ostream& out, TypeID ty);

    private:
        CompileUnit *_parent;

        ProcSignature _signature;
        std::string _id;

        RegisterID _next_vreg{VREG_START};

        std::vector<RegSlot> _params;
        std::vector<RegSlot> _frame;
        std::vector<RegSlot> _regs;

        std::vector<BasicBlock> _bblocks;
    };
}
