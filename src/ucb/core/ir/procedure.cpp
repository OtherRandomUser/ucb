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
            return new Operand(this, reg.data(), false);
        }

        reg = std::find_if(_frame.begin(), _frame.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (reg != _frame.end())
        {
            return new Operand(this, reg.data(), false);
        }

        auto bblock = std::find_if(begin(), end(), [&](auto& b) {
            return b.id() == id;
        });

        if (bblock != end())
        {
            return new Operand(this, bblock.data());
        }

        return nullptr;
    }
}
