#include <ucb/core/regalloc/graph-coloring.hpp>

#include <iostream>

#include <ucb/core/regalloc/interference-graph.hpp>

namespace  ucb
{
    void GraphColoringRegAlloc::run_on_procedure(std::shared_ptr<Procedure> proc, bool debug)
    {
        auto reg_classes = _target->load_reg_classes();

        if (debug)
        {
            std::cout << "register allocation for procedure: " << proc->id() << "\n\n";
        }

        for (auto& reg_class: reg_classes)
        {
            while (true)
            {
                // build
                auto k = reg_class.physical_regs.size();
                auto ig = build_interference_graph(*proc, reg_class.tys, _target);
                std::vector<IGNode> stack;
                std::vector<RegisterID> spills;

                if (debug)
                {
                    std::cout << "interference graph:\n";
                    ig.dump();
                }

                while (!ig.empty())
                {
                    // simplify
                    while (ig.can_simplify())
                    {
                        auto n = ig.pop_node(k);

                        if (!n.has_value())
                        {
                            break;
                        }
                        else
                        {
                            stack.push_back(std::move(n.value()));
                        }
                    }

                    // coalesce
                    if (ig.can_coalesce)
                    {
                        while (ig.coalesce(k));
                    }
                    // freeze
                    else if (ig.can_freeze())
                    {
                        ig.freeze();
                    }
                    // potential spill
                    else
                    {
                        auto n = ig.pop_highest_degree();
                        stack.push_back(std::move(n));
                    }
                }

                // select
                while (!stack.empty())
                {
                    auto n = std::move(stack.back());
                    stack.pop_back();

                    auto phys_reg = NO_REG;

                    for (auto reg: reg_class.physical_regs)
                    {
                        auto is_selected = false;

                        for (auto i: n.interferences)
                        {
                            auto& in = ig.get(i);

                            if (in.physical_register == reg)
                            {
                                is_selected = true;
                                break;
                            }
                        }

                        if (!is_selected)
                        {
                            phys_reg = reg;
                            break;
                        }
                    }

                    // successful selection
                    if (phys_reg != NO_REG)
                    {
                        n.physical_register = phys_reg;
                    }
                    else
                    // actual spill
                    {
                        if (n.keys.size() != 1)
                        {
                            std::cerr << "merged a node of significant degree" << std::endl;
                            abort();
                        }

                        spills.push_back(*n.keys.begin());
                    }

                    ig.push(std::move(n));
                }

                // successful selection for all instructions
                if (spills.empty())
                {
                    select_registers(proc, ig);
                    break;
                }
                // handle spills and try again
                else
                {
                    handle_spills(proc, spills);
                }
            }
        }
    }
}
