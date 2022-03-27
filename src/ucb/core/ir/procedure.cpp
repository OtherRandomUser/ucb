#include <ucb/core/ir/procedure.hpp>

#include <algorithm>

namespace ucb
{
    int Procedure::find_bblock(const std::string& id)
    {
        auto it = std::find_if(_bblocks.begin(), _bblocks.end(), [&](auto& b) {
            return b.id() == id;
        });

        if (it != _bblocks.end())
        {
            return std::distance(_bblocks.begin(), it);
        }

        return -1;
    }

    BasicBlock* Procedure::get_bblock(int idx)
    {
        assert(idx < _bblocks.size());
        return &_bblocks[idx];
    }

    int Procedure::add_bblock(std::string id)
    {
        auto idx = find_bblock(id);

        if (idx != -1)
        {
            return -1;
        }
        else
        {
            _bblocks.emplace_back(this, std::move(id));
            return _bblocks.size() - 1;
        }
    }

    Operand Procedure::operand_from_bblock(const std::string& id)
    {
        auto idx = find_bblock(id);

        if (idx == -1)
        {
            std::cerr << "basic block \"" << id << "\" not found\n";
            return Operand();
        }
        else
        {
            return Operand(this, idx);
        }
    }

    RegisterID Procedure::find_vreg(const std::string& id)
    {
        auto it = std::find_if(_regs.begin(), _regs.end(), [&](auto& r) {
            return r.second.id() == id;
        });

        if (it != _regs.end())
        {
            return it->first;
        }

        it = std::find_if(_frame.begin(), _frame.end(), [&](auto& r) {
            return r.second.id() == id;
        });

        if (it != _frame.end())
        {
            return it->first;
        }

        it = std::find_if(_params.begin(), _params.end(), [&](auto& r) {
            return r.second.id() == id;
        });

        if (it != _params.end())
        {
            return it->first;
        }

        return NO_REG;
    }

    const VirtualRegister* Procedure::get_register(RegisterID id) const
    {
        auto it = std::find_if(_regs.begin(), _regs.end(), [&](auto& r) {
            return r.first == id;
        });

        if (it != _regs.end())
        {
            return &it->second;
        }

        it = std::find_if(_frame.begin(), _frame.end(), [&](auto& r) {
            return r.first == id;
        });

        if (it != _frame.end())
        {
            return &it->second;
        }

        it = std::find_if(_params.begin(), _params.end(), [&](auto& r) {
            return r.first == id;
        });

        if (it != _params.end())
        {
            return &it->second;
        }

        return nullptr;
    }

    RegisterID Procedure::add_frame_slot(std::string id, TypeID ty)
    {
        auto rid = find_vreg(id);

        if (rid != NO_REG)
        {
            std::cerr << "virtual register \"" << id << "\" already exists\n";
            return NO_REG;
        }
        else
        {
            rid = _next_vreg++;
            _frame.emplace_back(rid, VirtualRegister(this, std::move(id), ty));
            return rid;
        }
    }

    RegisterID Procedure::add_vreg(std::string id, TypeID ty)
    {
        auto rid = find_vreg(id);

        if (rid != NO_REG)
        {
            std::cerr << "virtual register \"" << id << "\" already exists\n";
            return NO_REG;
        }
        else
        {
            rid = _next_vreg++;
            _regs.emplace_back(rid, VirtualRegister(this, std::move(id), ty));
            return rid;
        }
    }

    Operand Procedure::operand_from_vreg(const std::string& id, bool is_def)
    {
        auto rid = find_vreg(id);

        if (rid == NO_REG)
        {
            std::cerr << "virtual register \"" << id << "\" not found\n";
            return Operand();
        }
        else
        {
            return Operand(this, rid, is_def);
        }
    }

    void Procedure::dump(std::ostream& out)
    {
        _parent->dump_ty(out, _signature.ret());
        out << " @" << _id << "(";
        auto junc = "";

        for (auto& arg: _signature.args())
        {
            out << junc;
            _parent->dump_ty(out, arg.second);
            out << " " << arg.first;
            junc = ", ";
        }

        out << ")\n{\n";

        for (auto& bblock: _bblocks)
        {
            bblock.dump(out);
        }

        out << "}\n";
    }

    void Procedure::dump_ty(std::ostream& out, TypeID ty)
    {
        _parent->dump_ty(out, ty);
    }
}
