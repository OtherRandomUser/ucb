#include <ucb/core/ir/procedure.hpp>

#include <algorithm>

namespace ucb
{
    Operand* Procedure::operand_from_id(const std::string& id)
    {
        auto reg = std::find_if(_regs.begin(), _regs.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (reg != _regs.end())
        {
            return Operand::MakeVirtualReg(reg, false);
        }

        reg = std::find_if(_frame.begin(), _frame.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (reg != _frame.end())
        {
            return Operand::MakeVirtualReg(reg, false);
        }

        auto bblock = std::find_if(begin(), end(), [&](auto& b) {
            return b.id() == id;
        });

        if (b != end())
        {
            return Operand::MakeBasicBlock(bblock);
        }

        return nullptr;
    }
}
