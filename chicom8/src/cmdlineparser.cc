/*
 * commandlineparser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */


#include <vector>
#include <stdexcept>

#include "cmdlineparser.h"

#include <iostream>

bool CmdLineParser::parse_args( std::vector<std::string> arguments )
{
    if( arguments.empty() )
        return false;

    // Convert std::vector<std::string> to argc/argv
    std::vector<char*> argv_ptrs;
    argv_ptrs.reserve(arguments.size() + 1); // +1 for the program name
    std::string fake_prog_name = "chicom8";
    argv_ptrs.push_back(fake_prog_name.data());

    for (std::string& arg : arguments)
        argv_ptrs.push_back(arg.data());

    int const argc = static_cast<int>(argv_ptrs.size());
    char** argv = argv_ptrs.data();

	return parse_args(argc, argv);
}

bool CmdLineParser::parse_args(int argc, char** argv)
{
    cxxopts::Options options("chicom8", "A C-like compiler for Chip-8" );

    options.add_options()
        ("h,help", "Show this help message")
        ("v,verbose", "Verbose output")
        ("o", "Output file name", cxxopts::value<std::string>())
        ("source", "Input file name", cxxopts::value<std::string>());

    options.parse_positional({"source"});
    options.positional_help("<source>");
    options.show_positional_help();

    try {
        result = options.parse(argc, argv);
    }
    catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "Error parsing command line options: " << e.what() << '\n';
        std::cerr << options.help() << std::endl;
        return false;
    }

    if (result.count("help") != 0U) {
        std::cout << options.help() << '\n';
        return false;
    }

    if (result.count("source") == 0U) {
        std::cerr << "Error: Source file is required.\n";
        std::cerr << options.help() << '\n';
        return false;
    }

    return true;
}

std::string CmdLineParser::get_input_name() const
{
	return result["source"].as<std::string>();
}

std::string CmdLineParser::get_output_name() const
{
	if( result.count("o") != 0 )
        return result["o"].as<std::string>();

	std::string const source = result["source"].as<std::string>();
    std::filesystem::path const path(source);
    return path.stem().string() + ".ch8";
}

bool CmdLineParser::is_verbose() const
{
    return result["verbose"].as<bool>();
}

bool CmdLineParser::show_help() const
{
    return result.count("help") > 0;
}
