#include <ucb/core/ir/procedure.hpp>

#include <algorithm>

namespace ucb
{
    BasicBlock* Procedure::find_bblock(const std::string& id)
    {
        auto it = std::find_if(_bblocks.begin(), _bblocks.end(), [&](auto& b) {
            return b.id() == id;
        });

        if (it != _bblocks.end())
        {
            return &*it;
        }

        return nullptr;
    }

    BasicBlock* Procedure::add_bblock(std::string id)
    {
        auto ptr = find_bblock(id);

        if (prs != nullptr)
        {
            return nullptr;
        }
        else
        {
            _bblocks.emplace_back(this, std::move(id));
            return _bblocks.back();
        }
    }

    Operand* Procedure::operand_from_bblock(const std::string& id)
    {
        auto ptr = find_bblock(id);

        if (ptr == nullptr)
        {
            std::cerr << "basic block \"" << id << "\" not found\n";
            return nullptr;
        }
        else
        {
            return new Operand(this, ptr);
        }
    }

    VirtualRegister* Procedure::find_vreg(const std::string& id)
    {
        auto it = std::find_if(_regs.begin(), _regs.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (it != _regs.end())
        {
            return &*it;
        }

        it = std::find_if(_frame.begin(), _frame.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (it != _frame.end())
        {
            return &*it;
        }

        it = std::find_if(_params.begin(), _params.end(), [&](auto& r) {
            return r.id() == id;
        });

        if (it != _params.end())
        {
            return &*it;
        }

        return nullptr;
    }

    VirtualRegister& Procedure::add_frame_slot(std::string id, TypeID ty)
    {
        auto ptr = find_vreg(id);

        if (ptr != nullptr)
        {
            std::cerr << "virtual register \"" << id << "\" already exists\n";
            return nullptr;
        }
        else
        {
            _frame.emplace_back(this, std::move(id), ty);
            return _bblocks.back();
        }
    }

    VirtualRegister& Procedure::add_vreg(std::string id, TypeID ty)
    {
        auto ptr = find_vreg(id);

        if (ptr != nullptr)
        {
            std::cerr << "virtual register \"" << id << "\" already exists\n";
            return nullptr;
        }
        else
        {
            _regs.emplace_back(this, std::move(id), ty);
            return _bblocks.back();
        }
    }

    Operand* Procedure::operand_from_vreg(const std::string& id, bool is_def)
    {
        auto ptr = find_vreg(id);

        if (ptr == nullptr)
        {
            std::cerr << "virtual register \"" << id << "\" not found\n";
            return nullptr;
        }
        else
        {
            return new Operand(this, ptr, is_def);
        }
    }
}
