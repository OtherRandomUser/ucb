#pragma once

#include <memory>

#include <ucb/core/ir/basic-block.hpp>
#include <ucb/core/ir/ilist.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
    class ProcSignature : public IList<VirtualRegister>
    {
    public:
        ProcSignature(Type *ty):
            _ty{ty}
        {
        }

        ~ProcSignature()
        {
            if (_ty)
            {
                delete _ty;
            }
        }

        const Type* ret() { return _ty; };

    private:
        Type *_ty;
    };

    class ProcFrame : public IList<VirtualRegister>
    {
    };

    class Procedure : public IList<BasicBlock>
    {
    public:
        Procedure(CompileUnit *parent, std::unique_ptr<ProcSignature> signature):
            _parent{parent},
            _signature(std::move(signature))
        {
        }

        ProcSignature& signature() { return *_signature; }
        ProcFrame& frame() { return *_frame; }

        BasicBlock& entry()
        {
            assert(size());
            return *begin();
        }

    private:
        CompileUnit *_parent;
        BasicBlock *_entry;
        std::unique_ptr<ProcSignature> _signature;
        std::unique_ptr<ProcFrame> _frame;
    };
}
