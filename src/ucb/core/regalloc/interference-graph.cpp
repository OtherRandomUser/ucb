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
        std::cout << "keys: { ";
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
            auto node_a = get(a);
            node_a.interferences.insert(b);

            auto node_b = get(b);
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

    InterferenceGraph build_interference_graph(Procedure& proc, const std::vector<TypeID>& tys, std::shared_ptr<Target> target)
    {
        InterferenceGraph res;

        for (auto& bblock: proc.bblocks())
        {
            std::vector<RegisterID> live_regs;

            for (auto pair: bblock.live_outs())
            {
                auto it = std::find_if(
                    tys.begin(),
                    tys.end(),
                    [&](auto ty){ return ty.val == pair.second.val; });

                if (it != tys.end())
                {
                    live_regs.push_back(pair.first);
                }
            }

            auto& insts = bblock.machine_insts();

            for (auto it = insts.rbegin(); it != insts.rend(); it++)
            {
                // add/remove live regs
                for (auto& opnd: it->opnds)
                {
                    if (opnd.is_def)
                    {
                        // remove def from live regs
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        auto it = std::find(live_regs.begin(), live_regs.end(), reg);

                        if (it != live_regs.end())
                        {
                            live_regs.erase(it);
                        }
                    }
                    else if (opnd.kind == MachineOperand::Register)
                    {
                        // add use to live regs
                        auto reg = std::bit_cast<RegisterID>(opnd.val);
                        auto it = std::find(live_regs.begin(), live_regs.end(), reg);

                        if (it == live_regs.end())
                        {
                            live_regs.push_back(reg);
                        }
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
