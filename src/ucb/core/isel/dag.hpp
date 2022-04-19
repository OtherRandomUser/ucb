#pragma once

#include <bit>
#include <memory>
#include <ostream>
#include <vector>

#include <ucb/core/ir/compile-unit.hpp>
#include <ucb/core/ir/instruction.hpp>
#include <ucb/core/ir/type.hpp>

namespace ucb
{
    enum DagDefKind
    {
        DDK_NONE,
        DDK_REG,
        DDK_MEM,
        DDK_IMM,
        DDK_ADDR,
        DDK_ENTRY,
        DDK_EXIT,
    };

    class DagNode
    {
    public:
        friend class Dag;

        DagNode(int og_order, InstrOpcode opc, DagDefKind kind, TypeID ty, std::string id):
            _og_order{og_order},
            _opc{opc},
            _kind{kind},
            _ty{ty},
            _id(std::move(id))
        {
        }

        bool is_leaf() const { return _args.empty() && _opc == InstrOpcode::OP_NONE; }
        DagDefKind kind() const { return _kind; }
        InstrOpcode opc() const { return _opc; }
        TypeID ty() const { return _ty; }
        RegisterID& reg() { return _reg; }
        std::uint64_t& imm_val() { return _imm_val; }
        std::uint64_t& mem_id() { return _mem_id; }
        const std::string& id() const { return _id; }
        float cost() const { return _cost; }

        std::vector<std::shared_ptr<DagNode>>& args() { return _args; }
        std::vector<std::shared_ptr<DagNode>>& selected_args() { return _selected_args; }
        std::list<MachineInstruction>& selected_insts() { return _selected_insts; }

        DagNode& add_arg(std::shared_ptr<DagNode> arg)
        {
            _args.push_back(std::move(arg));
            return *this;
        }

        DagNode& add_selected_arg(std::shared_ptr<DagNode> arg)
        {
            _selected_args.push_back(std::move(arg));
            return *this;
        }

        void add_selected_args(std::vector<std::shared_ptr<DagNode>> args)
        {
            _selected_args.insert(
                _selected_args.end(),
                std::make_move_iterator(args.begin()),
                std::make_move_iterator(args.end())
            );
        }

        void add_selected_insts(std::list<MachineInstruction> insts)
        {
            _selected_insts.insert(
                _selected_insts.end(),
                std::make_move_iterator(insts.begin()),
                std::make_move_iterator(insts.end())
            );
        }

        void dump(std::ostream& out, CompileUnit& context);
        void dump(std::ostream& out, CompileUnit& context, const std::string& pre);

    private:
        int _og_order;
        InstrOpcode _opc;
        DagDefKind _kind;
        TypeID _ty;
        std::string _id;

        MachineOpc _mopc{MOP_NONE};
        RegisterID _reg{NO_REG};
        // int _imm_id{0};
        std::uint64_t _imm_val{0};
        std::uint64_t _mem_id{0};

        float _cost{0};
        int _uses{0};

        std::vector<std::shared_ptr<DagNode>> _args;
        std::vector<std::shared_ptr<DagNode>> _selected_args;
        std::list<MachineInstruction> _selected_insts;
    };

    /*
    struct DagImm
    {
        enum Kind
        {
            IMM_INT,
            IMM_UINT,
            IMM_FLOAT
        } kind;

        std::uint64_t val;
    };
    */

    struct DagMem
    {
        std::uint64_t size;
    };

    class Dag
    {
    public:
        // std::shared_ptr<DagNode> entry() { return _entry; }
        // std::shared_ptr<DagNode> exit() { return _exit; }

        std::shared_ptr<DagNode> get_register(RegisterID id, TypeID ty);
        //std::shared_ptr<DagNode> get_int_imm(long int val, TypeID ty);
        //std::shared_ptr<DagNode> get_uint_imm(unsigned long val, TypeID ty);
        //std::shared_ptr<DagNode> get_float_imm(double val, TypeID ty);

        template<typename T>
        std::shared_ptr<DagNode> get_imm(T val, TypeID ty)
        {
            auto n = std::make_shared<DagNode>(-1, InstrOpcode::OP_NONE, DagDefKind::DDK_IMM, ty, "");
            n->_imm_val = std::bit_cast<std::uint64_t>(val);
            return n;
        }

        void add_def(std::shared_ptr<DagNode> def)
        {
            _all_nodes.push_back(std::move(def));
        }

        void add_root_def(std::shared_ptr<DagNode> def)
        {
            _root_nodes.push_back(def);
            add_def(std::move(def));
        }

        // void add_live_in(RegisterID id, TypeID ty)
        // {
        //     _live_ins.emplace_back(id, ty);
        // }

        std::vector<std::shared_ptr<DagNode>>& root_nodes() { return _root_nodes; }

        void dump(std::ostream& out, CompileUnit& context);

    private:
        // std::shared_ptr<DagNode> _entry;
        // std::shared_ptr<DagNode> _exit;

        std::vector<std::shared_ptr<DagNode>> _all_nodes;
        std::vector<std::shared_ptr<DagNode>> _root_nodes;
        // std::vector<std::pair<RegisterID, TypeID>> _live_ins;

        // std::vector<DagImm> _imms;
        std::vector<DagMem> _mems;
    };
}
