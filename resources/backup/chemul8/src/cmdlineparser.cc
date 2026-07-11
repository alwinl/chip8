/*
 * cmdline_processor.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "cmdlineparser.h"

#include <iostream>

bool CmdLineParser::parse_args( std::vector<std::string> arguments )
{
    if( arguments.empty() )
		return false;

    // Convert std::vector<std::string> to argc/argv
    std::vector<char*> argv_ptrs;
    argv_ptrs.reserve(arguments.size() + 1); // +1 for the program name
    std::string fake_prog_name = "chasem8";
    argv_ptrs.push_back(fake_prog_name.data());

    for (std::string& arg : arguments)
        argv_ptrs.push_back(arg.data());

    int argc = static_cast<int>(argv_ptrs.size());
    char** argv = argv_ptrs.data();

	return parse_args(argc, argv);
}


bool CmdLineParser::parse_args( int argc, char ** argv )
{
    cxxopts::Options options("chemul8", "My chip8 emulator");

    options.add_options()
        ("C,chip8", "Emulate original Chip8")
        ("X,xochip", "Emulate the XOChip8")
        ("S,schip", "Emulate the Super Chip8")
        ("h,help", "Print usage")
        ("romfile", "CHIP-8 ROM file to load", cxxopts::value<std::string>());

    options.parse_positional({"romfile"});
    options.positional_help("romfile");
	options.show_positional_help();

	try {
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::exception& e) {
		std::cerr << "Error parsing command line options: " << e.what() << std::endl;
		std::cerr << options.help() << std::endl;
		return false;
	}

    if( result.count("help") ) {
        std::cout << options.help() << std::endl;
        return false;
    }

    if( !result.count("romfile") ) {
        std::cout << options.help() << std::endl;
        std::cout << "Missing romfile" << std::endl;
        return false;
    }

	return true;
}

std::string CmdLineParser::get_program()
{
	return result["romfile"].as<std::string>();
}


// Quirks::eChipType CmdlineProcessor::decode_chip_type( cxxopts::ParseResult result )
// {
//     if( result.count( "chip8" ) )
//         return Quirks::eChipType::CHIP8;

//     if( result.count( "xochip") )
//         return Quirks::eChipType::XOCHIP;

//     if( result.count( "schip") )
//         return Quirks::eChipType::SCHIP;

//     return Quirks::eChipType::CHIP8;
// }

