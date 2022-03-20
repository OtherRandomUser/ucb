#include <ucb/core/ir/procedure.hpp>

#include <algorithm>

namespace ucb
{
    BasicBlock& Procedure::add_bblock(std::string id)
    {
        auto it = std::find_if(_bblocks.begin(), _bblocks.end(), [&](auto& b) {
            return b.id() == id;
        });

        if (it != _bblocks.end())
        {
            return *it;
        }
        else
        {
            _bblocks.emplace_back(this, std::move(id));
            return _bblocks.back();
        }
    }

    Operand* Procedure::operand_from_bblock(std::string id)
    {
        return new Operand(this, &add_bblock(std::move(id));
    }

    Operand* Procedure::operand_from_vreg(const std::string& id, bool is_def)
    {
        auto it = std::find_if(_regs.begin(), _regs.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (it != _regs.end())
        {
            return new Operand(this, &*it, is_def);
        }

        it = std::find_if(_frame.begin(), _frame.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (it != _frame.end())
        {
            return new Operand(this, &*it, false);
        }

        it = std::find_if(_params.begin(), _params.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (it != _params.end())
        {
            return new Operand(this, &*it, false);
        }

        return nullptr;
    }
}
