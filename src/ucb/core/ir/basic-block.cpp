#include <ucb/core/ir/basic-block.hpp>

namespace ucb
{
    CompileUnit* BasicBlock::context()
    {
        return _parent->context();
    }

    void BasicBlock::clear_dataflow()
    {
        _predecessors.clear();
        _successors.clear();
        _live_ins.clear();
        _live_outs.clear();
    }

    bool BasicBlock::compute_live_ins()
    {
        std::vector<std::pair<RegisterID, TypeID>> new_live_ins;

        for (auto bblock: _successors)
        {
            bblock->compute_live_ins();

            new_live_ins.insert(
                new_live_ins.begin(),
                bblock->_live_ins.begin(),
                bblock->_live_ins.end());
        }

        for (auto it = _insts.rbegin(); it != _insts.rend(); it++)
        {
            auto& opnds = it->opnds();

            for (auto& opnd: opnds)
            {
                if (opnd.is_def())
                {
                    auto reg = opnd.get_virtual_reg();
                    auto it = std::remove_if(
                        new_live_ins.begin(),
                        new_live_ins.end(),
                        [reg](auto pair) { return pair.first == reg; });

                    new_live_ins.erase(it);
                }
                else if (opnd.kind() == OperandKind::OK_VIRTUAL_REG)
                {
                    auto reg = opnd.get_virtual_reg();
                    auto live_in = std::find_if(
                        new_live_ins.begin(),
                        new_live_ins.end(),
                        [reg](auto pair) { return pair.first == reg; });

                    if (live_in == new_live_ins.end())
                    {
                        new_live_ins.emplace_back(reg, opnd.ty());
                    }
                }
            }
        }

        for (auto li: new_live_ins)
        {
            auto it = std::find(_live_ins.begin(), _live_ins.end(), li);

            if (it == _live_ins.end())
            {
                _live_ins = std::move(new_live_ins);
                return true;
            }
        }

        return false;
    }

    void BasicBlock::compute_live_outs()
    {
        for (auto& bblock: _successors)
        {
            for (auto reg: bblock->_live_ins)
            {
                auto it = std::find(
                    _live_outs.begin(),
                    _live_outs.end(),
                    reg);

                if (it == _live_outs.end())
                {
                    _live_outs.push_back(reg);
                }
            }
        }
    }

    void BasicBlock::dump(std::ostream& out)
    {
        out << _id << ":\n";

        if (!_predecessors.empty())
        {
            out << "\tpredecessors:\n";

            for (auto pred: _predecessors)
            {
                out << "\t\t" << pred->id() << "\n";
            }

            out << "\n";
        }

        if (!_live_ins.empty())
        {
            out << "\tlive ins:\n";

            for (auto [id, ty]: _live_ins)
            {
                out << "\t\t";
                _parent->dump_ty(out, ty);
                out << " { " << id.val << ", " << id.size << " }\n";
            }

            out << "\n";
        }

        for (auto& inst: _insts)
        {
            inst.dump(out);
        }

        out << '\n';
    }

    void BasicBlock::dump_ty(std::ostream& out, TypeID ty)
    {
        _parent->dump_ty(out, ty);
    }

    bool BasicBlock::reg_is_live_out(RegisterID reg)
    {
        if (reg == NO_REG) { return false; }

        auto it = std::find_if(
            _live_outs.begin(),
            _live_outs.end(),
            [reg](auto r) { return r.first == reg; });

        return it != _live_outs.end();
    }
}
