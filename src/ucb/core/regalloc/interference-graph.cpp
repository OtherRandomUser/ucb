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

    void InterferenceGraph::add_interference(RegisterID a, RegisterID b, bool is_rr_move)
    {
        auto node_a = std::find_if(nodes.begin(), nodes.end(), [&](auto& n)
        {
            auto it = std::find(n.keys.begin(), n.keys.end(), a);
            return it != n.keys.end();
        });

        if (node_a == nodes.end())
        {
            nodes.push_back({
                .keys = { a },
                .interferences = { b },
                .is_move_adjacent = is_rr_move
            });
        }
        else
        {
            auto it = std::find(node_a->interferences.begin(), node_a->interferences.end(), b);

            if (it == node_a->interferences.end())
            {
                node_a->interferences.push_back(b);
            }

            if (!node_a->is_move_adjacent)
            {
                node_a->is_move_adjacent = is_rr_move;
            }
        }

        auto node_b = std::find_if(nodes.begin(), nodes.end(), [&](auto& n)
        {
            auto it = std::find(n.keys.begin(), n.keys.end(), b);
            return it != n.keys.end();
        });


        if (node_b == nodes.end())
        {
            nodes.push_back({
                .keys = { b },
                .interferences = { a },
                .is_move_adjacent = is_rr_move
            });
        }
        else
        {
            auto it = std::find(node_b->interferences.begin(), node_b->interferences.end(), a);

            if (it == node_b->interferences.end())
            {
                node_b->interferences.push_back(a);
            }

            if (!node_b->is_move_adjacent)
            {
                node_b->is_move_adjacent = is_rr_move;
            }
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

                auto is_rr_move = target->is_rr_move(it->opc);

                // add interferences
                for (auto& opnd: it->opnds)
                {
                    if (!opnd.is_def && opnd.kind == MachineOperand::Register)
                    {
                        auto a = std::bit_cast<RegisterID>(opnd.val);

                        for (auto b: live_regs)
                        {
                            if (a != b && !res.is_interference(a, b))
                            {
                                res.add_interference(a, b, is_rr_move);
                            }
                        }
                    }
                }
            }
        }

        return res;
    }
}
