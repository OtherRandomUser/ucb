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
                    while (true)
                    {
                        std::cout << "simplify" << std::endl;
                        auto n = ig.pop_node(k);

                        if (n.has_value())
                        {
                            std::cout << "popped node:" << std::endl;
                            ig.dump();
                            stack.push_back(std::move(n.value()));
                        }
                        else
                        {
                            break;
                        }
                    }

                    // coalesce
                    auto mvs = ig.coalesce(k);

                    if (!mvs.empty())
                    {
                        std::cout << "coalesced removing " << mvs.size() << " moves:" << std::endl;
                        ig.dump();

                        for (auto ptr: mvs)
                        {
                            for (auto& bblock: proc->bblocks())
                            {
                                auto& insts = bblock.machine_insts();
                                auto it = std::find_if(
                                    insts.begin(),
                                    insts.end(),
                                    [ptr](auto& inst)
                                    {
                                        return &inst == ptr;
                                    });

                                if (it != insts.end())
                                {
                                    insts.erase(it);
                                    break;
                                }
                            }

                            std::cout << "proc after removeing moves:\n";
                            _target->dump_proc(*proc, std::cout);
                        }
                    }
                    // freeze
                    else if (!ig.freeze_move())
                    {
                        std::cout << "potential spill?" << std::endl;
                        // potential spill
                        if (!ig.empty() && !ig.can_simplify())
                        {
                            std::cout << "potential spill!" << std::endl;
                            auto n = ig.pop_highest_degree();
                            stack.push_back(std::move(n.value()));
                        }
                    }
                }

                std::cout << "select phase" << std::endl;

                // select
                while (!stack.empty())
                {
                    auto n = std::move(stack.back());
                    stack.pop_back();

                    auto phys_reg = NO_REG;

                    if (n.physical_register != NO_REG)
                    {
                        phys_reg = n.physical_register;
                    }
                    else
                    {
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
                                phys_reg.size = n.keys.begin()->size;
                                break;
                            }
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

                    ig.push_node(std::move(n));
                }

                std::cout << "ig after selection:\n";
                ig.dump();

                // successful selection for all instructions
                if (spills.empty())
                {
                    select_registers(*proc, ig);
                    break;
                }
                // handle spills and try again
                else
                {
                    std::cerr << "spills are not implemented yet" << std::endl;
                    abort();
                    // handle_spills(proc, spills);
                }
            }
        }

        if (debug)
        {
            std::cout << "procedure " << proc->id() << " after register allocation\n\n";

            for (auto& bblock: proc->bblocks())
            {
                _target->dump_bblock(bblock, std::cout);
            }
        }
    }

    void GraphColoringRegAlloc::select_registers(Procedure& proc, InterferenceGraph& ig)
    {
        for (auto& bblock: proc.bblocks())
        {
            for (auto& inst: bblock.machine_insts())
            {
                for (auto& opnd: inst.opnds)
                {
                    if (opnd.kind != MachineOperand::Register) { continue; }

                    auto reg = std::bit_cast<RegisterID>(opnd.val);

                    if (!is_physical_reg(reg))
                    {
                        auto p = ig.get(reg).physical_register;
                        opnd.val = std::bit_cast<std::uint64_t>(p);
                    }
                }
            }
        }
    }
}
