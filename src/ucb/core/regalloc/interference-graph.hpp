#pragma once

#include <optional>

#include <ucb/core/target.hpp>
#include <ucb/core/ir/procedure.hpp>
#include <ucb/core/ir/type.hpp>
#include <ucb/core/ir/virtual-register.hpp>

namespace ucb
{
    struct IGNode
    {
        std::vector<RegisterID> keys;
        std::vector<RegisterID> interferences;

        RegisterID physical_register{NO_REG};

        bool no_spill{false};
        bool is_move_adjacent{false};

        bool interferes_with(const IGNode& other);
    };

    struct InterferenceGraph
    {
        std::vector<IGNode> nodes;

        bool is_interference(RegisterID a, RegisterID b);
        void add_interference(RegisterID a, RegisterID b, bool is_rr_move);

        // IGNode& get(RegisterID);
        // void merge(RegisterID a, RegisterID b);
        // void clean_dead_nodes();
    };

    InterferenceGraph build_interference_graph(Procedure& proc, const std::vector<TypeID>& tys, std::shared_ptr<Target> target);
}
