#include <ucb/core/config.hpp>

#include <iostream>

#include <ucb/core/pass-manager.hpp>
#include <ucb/core/backend/x64.hpp>
#include <ucb/core/isel/dp-isel.hpp>
#include <ucb/core/regalloc/graph-coloring.hpp>
#include <ucb/frontend/lexer.hpp>
#include <ucb/frontend/parser.hpp>

#include <boost/program_options.hpp>

using namespace ucb;

namespace po = boost::program_options;

std::unique_ptr<PassManager> make_pass_manager()
{
    std::vector<std::unique_ptr<Pass>> passes;
    auto target = std::make_shared<x64::X64Target>();

    auto isel = std::make_unique<DynamicISel>(target);
    auto regalloc = std::make_unique<GraphColoringRegAlloc>(target);
    auto target_machine = std::make_unique<TargetMachine>(TargetArch::ARCH_X64, std::move(isel), std::move(regalloc), target);
    return std::make_unique<PassManager>(std::move(passes), std::move(target_machine));
}

int main(int argc, char **argv)
{
    std::string input_file;

    po::options_description desc("UCB Intermediate Representaiton Compiler");
    desc.add_options()
        ("help,h", "print this message")
        ("input-file", po::value<std::string>(&input_file)->required(), "file to be compiled")
    ;

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if (!vm.count("input-file"))
    {
        std::cout << "an input file must be provided" << std::endl;
        return EXIT_FAILURE;
    }

    po::notify(vm);

    auto context = std::make_shared<CompileUnit>();
    frontend::Parser parser(vm["input-file"].as<std::string>(), context, false, false);

    if (!parser.parse_unit())
    {
        std::cerr << "parse failure!!!\n";
        return EXIT_FAILURE;
    }

    auto pm = make_pass_manager();
    pm->apply(context);

    return EXIT_SUCCESS;
}
