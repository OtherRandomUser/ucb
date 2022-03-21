#include <ucb/core/ir/basic-block.hpp>

namespace ucb
{
    void BasicBlock::dump(std::ostream& out) const
    {
        out << _id << ":\n";

        auto it = begin();

        while (it != end())
        {
            it->dump(out);
            it++;
        }

        out << '\n';
    }

    void BasicBlock::dump_ty(std::ostream& out, TypeID ty) const
    {
        _parent->dump_ty(out, ty);
    }
}
