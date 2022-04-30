#pragma once

#include <set>
#include <optional>

#include <ucb/core/target.hpp>
#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
    struct IGNode
    {
        std::set<RegisterID> keys;
        std::set<RegisterID> interferences;
        std::set<MachineInstruction*> moves;

        RegisterID physical_register{NO_REG};

        bool no_spill{false};

        bool interferes_with(const IGNode& other);

        void dump();
    };

    struct InterferenceGraph
    {
        std::vector<IGNode> nodes;

        bool is_interference(RegisterID a, RegisterID b);
        void add_interference(RegisterID a, RegisterID b);

        IGNode& get(RegisterID key);
        int get_neighbor_count(IGNode& node, int k);

        bool briggs(IGNode& a, IGNode& b, int k);

        bool empty();
        bool can_simplify();
        // bool can_coalesce();

        std::set<MachineInstruction*> coalesce(int k);
        bool freeze_move();
        std::optional<IGNode> pop_node(int k);
        std::optional<IGNode> pop_highest_degree();
        void push_node(IGNode node);

        void dump();

        // void merge(RegisterID a, RegisterID b);
        // void clean_dead_nodes();
    };

    InterferenceGraph build_interference_graph(Procedure& proc, const std::vector<TypeID>& tys, std::shared_ptr<Target> target);
}
