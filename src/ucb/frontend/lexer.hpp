#pragma once

#include <ucb/core/config.hpp>

#include <string>
#include <iostream>

#include <ucb/frontend/span-reader.hpp>

namespace ucb::frontend
{
    class Lexer
    {
    public:
        Lexer(std::string filename, bool print_debug):
            _filename(std::move(filename)),
            _print_debug(print_debug)
        {
            std::cout << "batatas! " << _filename << std::endl;
        }

    private:
        std::string _filename;
        bool _print_debug;
    };
}
