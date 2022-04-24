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

    void IGNode::add_move(MachineInstruction *move)
    {
        auto it = std::find(moves.begin(), moves.end(), move);
        if (it == moves.end()) { moves.push_back(move); }
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
            auto a_it = std::find(node_a.interferences.begin(), node_a.interferences.end(), b);

            if (a_it == node_a.interferences.end())
            {
                node_a.interferences.push_back(b);
            }

            auto node_b = get(b);
            auto b_it = std::find(node_b.interferences.begin(), node_b.interferences.end(), a);

            if (b_it == node_b.interferences.end())
            {
                node_b.interferences.push_back(a);
            }
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

            return nodes.back();
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
                            node.add_move(move_inst);
                        }
                    }
                }
            }
        }

        return res;
    }
}
