#include <ucb/core/ir/basic-block.hpp>

namespace ucb
{
    void BasicBlock::dump(std::ostream& out)
    {
        out << _id << ":\n";

        auto it = _insts.begin();

        while (it != _insts.end())
        {
            it->dump(out);
            it++;
        }

        out << '\n';
    }

    void BasicBlock::dump_ty(std::ostream& out, TypeID ty)
    {
        _parent->dump_ty(out, ty);
    }
}
