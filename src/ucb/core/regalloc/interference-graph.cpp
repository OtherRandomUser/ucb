#include <ucb/core/regalloc/interference-graph.hpp>

namespace ucb
{
    bool IGNode::interferes_with(const IGNode& other)
    {
        for (auto key: other.keys)
        {
            auto it = std::find(interferences.begin(), interferences.end(), key);

            if (it != interferences.end())
            {
                return true;
            }
        }

        return false;
    }

    void IGNode::dump()
    {
        std::cout << "\tkeys: { ";
        std::string junc = "";

        for (auto key: keys)
        {
            std::cout << junc << key.val;
            junc = ", ";
        }

        std::cout << " }, interferences: { ";
        junc = "";

        for (auto interference: interferences)
        {
            std::cout << junc << interference.val;
            junc = ", ";
        }

        std::cout << " }, is move ajacent: " << (moves.empty() ? "false" : "true");

        if (physical_register != NO_REG)
        {
            std::cout << ", physical reg: " << physical_register.val;
        }

        std::cout << std::endl;
    }

    bool InterferenceGraph::is_interference(RegisterID a, RegisterID b)
    {
        auto node_a = std::find_if(nodes.begin(), nodes.end(), [&](auto& n)
        {
            auto it = std::find(n.keys.begin(), n.keys.end(), a);
            return it != n.keys.end();
        });

        if (node_a == nodes.end())
        {
            return false;
        }

        auto node_b = std::find_if(nodes.begin(), nodes.end(), [&](auto& n)
        {
            auto it = std::find(n.keys.begin(), n.keys.end(), b);
            return it != n.keys.end();
        });

        if (node_a == nodes.end())
        {
            return false;
        }

        return node_a->interferes_with(*node_b);
    }

    void InterferenceGraph::add_interference(RegisterID a, RegisterID b)
    {
        if (a != b && !is_interference(a, b))
        {
            std::cout << "add interference " << a.val << " and " << b.val << std::endl;
            auto& node_a = get(a);
            node_a.interferences.insert(b);

            auto& node_b = get(b);
            node_b.interferences.insert(a);
        }
    }

    IGNode& InterferenceGraph::get(RegisterID key)
    {
        auto node = std::find_if(nodes.begin(), nodes.end(), [&](auto& n)
        {
            auto it = std::find(n.keys.begin(), n.keys.end(), key);
            return it != n.keys.end();
        });

        if (node == nodes.end())
        {
            nodes.push_back({
                .keys = { key }
            });

            auto& res = nodes.back();

            if (is_physical_reg(key))
            {
                res.physical_register = key;
            }

            return res;
        }
        else
        {
            return *node;
        }
    }

    bool InterferenceGraph::empty()
    {
        return nodes.empty();
    }
/*
    bool InterferenceGraph::can_simplify()
    {
        if (nodes.empty()) { return false; }

        auto it = std::find_if(
            nodes.begin(),
            nodes.end(),
            [](auto& n){ return n.moves.empty(); });

        return it != nodes.end();
    }

    bool InterferenceGraph::can_coalesce()
    {
        if (nodes.empty()) { return false; }

        auto it = std::find_if(
            nodes.begin(),
            nodes.end(),
            [](auto& n){ return n.moves.empty(); });

        return it == nodes.end();
    }
*/
    MachineInstruction* InterferenceGraph::coalesce(int k)
    {
        if (nodes.empty()) { return nullptr; }

        auto it = nodes.begin();

        while (it != nodes.end())
        {
            if (!it->moves.empty())
            {
                for (auto mv: it->moves)
                {
                    auto itb = std::find_if(
                        it + 1,
                        nodes.end(),
                        [mv](auto& n)
                        {
                            //auto itc = std::find(n.moves.begin(), n.moves.end(), mv);
                            //return itc != n.moves.end();
                            return n.moves.contains(mv);
                        });

                    if (itb == nodes.end())
                    {
                        std::cerr << "reference to move instruction appears only once" << std::endl;
                        abort();
                    }

                    // if the two registers interfere with each other we will never be able to merge them
                    if (it->interferes_with(*itb))
                    {
                        it->moves.erase(mv);
                        itb->moves.erase(mv);
                        continue;
                    }

                    // check if both nodes are already colored
                    if (it->physical_register != NO_REG
                        && itb->physical_register != NO_REG)
                    {
                        it->moves.erase(mv);
                        itb->moves.erase(mv);
                        continue;
                    }

                    // briggs
                    if (it->interferences.size() < k
                        && itb->interferences.size() < k)
                    {
                        // merge the two nodes
                        it->keys.merge(std::move(itb->keys));
                        it->interferences.merge(std::move(itb->interferences));
                        it->moves.merge(std::move(itb->moves));
                        it->moves.erase(mv);

                        if (itb->physical_register != NO_REG)
                        {
                            it->physical_register = itb->physical_register;
                        }

                        nodes.erase(itb);
                        return mv;
                    }
                }
            }

            ++it;
        }

        return nullptr;
    }

    bool InterferenceGraph::freeze_move()
    {
        if (nodes.empty()) { return false; }

        auto it = nodes.begin();

        while (it != nodes.end())
        {
            if (!it->moves.empty())
            {
                for (auto mv: it->moves)
                {
                    auto itb = std::find_if(
                        it + 1,
                        nodes.end(),
                        [mv](auto& n)
                        {
                            //auto itc = std::find(n.moves.begin(), n.moves.end(), mv);
                            //return itc != n.moves.end();
                            return n.moves.contains(mv);
                        });

                    if (itb == nodes.end())
                    {
                        std::cerr << "reference to move instruction appears only once" << std::endl;
                        abort();
                    }

                    it->moves.erase(mv);
                    itb->moves.erase(mv);
                    return true;
                }
            }
        }

        return false;
    }

    // TODO select colored nodes last
    std::optional<IGNode> InterferenceGraph::pop_node(int k)
    {
        if (nodes.empty())
            return std::nullopt;

        auto it = nodes.begin();
        auto selected = nodes.end();
        auto degree = -1;

        while (it != nodes.end())
        {
            auto it_degree = it->interferences.size();

            if (it_degree > degree
                && it_degree < k
                && it->moves.empty())
            {
                selected = it;
                degree = it_degree;
            }

            ++it;
        }
        
        if (selected == nodes.end())
        {
            return std::nullopt;
        }
        else
        {
            it = nodes.begin();

            while (it != nodes.end())
            {
                if (it == selected) { continue; }

                for (auto key: selected->keys)
                {
                    it->interferences.erase(key);
                }

                ++it;
            }

            auto opt = std::optional<IGNode>{*selected};
            nodes.erase(selected);
            return opt;
        }
    }

    std::optional<IGNode> InterferenceGraph::pop_highest_degree()
    {
        return pop_node(10000);
    }

    InterferenceGraph build_interference_graph(Procedure& proc, const std::vector<TypeID>& tys, std::shared_ptr<Target> target)
    {
        InterferenceGraph res;

        std::cout << "building interference graph:" << std::endl;

        for (auto& bblock: proc.bblocks())
        {
            std::cout << "bblock" << std::endl;
            std::set<RegisterID> live_regs;

            std::cout << "adding live outs" << std::endl;
            for (auto pair: bblock.live_outs())
            {
                auto it = std::find_if(
                    tys.begin(),
                    tys.end(),
                    [&](auto ty){ return ty.val == pair.second.val; });

                if (it != tys.end())
                {
                    std::cout << "added reg " << pair.first.val << " from live outs" << std::endl;
                    live_regs.insert(pair.first);
                }
            }

            auto& insts = bblock.machine_insts();

            std::cout << "instructions:" << std::endl;
            for (auto it = insts.rbegin(); it != insts.rend(); it++)
            {
                std::cout << "inst" << std::endl;
                // add/remove live regs
                for (auto& opnd: it->opnds)
                {
                    std::cout << "opnd is def = " << opnd.is_def << " & is use = " << opnd.is_use << std::endl;
                    if (opnd.is_def && !opnd.is_use)
                    {
                        // remove def from live regs
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        live_regs.erase(reg);
                        std::cout << "remove live reg " << reg.val << std::endl;
                        std::cout << "live regs:";
                        for (auto lr: live_regs) { std::cout << " " << lr.val; }
                        std::cout << std::endl;
                    }
                    else if (opnd.kind == MachineOperand::Register)
                    {
                        // add use to live regs
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        live_regs.insert(reg);
                        std::cout << "add live reg " << reg.val << std::endl;
                        std::cout << "live regs:";
                        for (auto lr: live_regs) { std::cout << " " << lr.val; }
                        std::cout << std::endl;
                    }
                }

                // add interferences
                for (auto& opnd: it->opnds)
                {
                    if (!opnd.is_def && opnd.kind == MachineOperand::Register)
                    {
                        auto a = std::bit_cast<RegisterID>(opnd.val);

                        for (auto b: live_regs)
                        {
                            res.add_interference(a, b);
                        }
                    }
                }

                // add moves
                if (target->is_rr_move(it->opc))
                {
                    auto move_inst = &(*it);

                    for (auto& opnd: it->opnds)
                    {
                        if (opnd.kind == MachineOperand::Register)
                        {
                            auto key = std::bit_cast<RegisterID>(opnd.val);
                            auto& node = res.get(key);
                            node.moves.insert(move_inst);
                        }
                    }
                }
            }
        }

        return res;
    }

    void InterferenceGraph::dump()
    {
        for (auto& node: nodes)
        {
            node.dump();
        }

        std::cout << std::endl;
    }
}
