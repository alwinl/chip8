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

#include "cmdline_processor.h"

#include <iostream>

CmdlineProcessor::CmdlineProcessor( int argc, char ** argv )
{
    cxxopts::Options options = build_options();

    auto result = options.parse(argc, argv);

    if( !validate( options, result ) )
        return;

    chip_type = decode_chip_type( result );
    program = result["romfile"].as<std::string>();
}

cxxopts::Options CmdlineProcessor::build_options()
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

    return options;
}

bool CmdlineProcessor::validate( cxxopts::Options options, cxxopts::ParseResult result )
{
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

Quirks::eChipType CmdlineProcessor::decode_chip_type( cxxopts::ParseResult result )
{
    if( result.count( "chip8" ) )
        return Quirks::eChipType::CHIP8;

    if( result.count( "xochip") )
        return Quirks::eChipType::XOCHIP;

    if( result.count( "schip") )
        return Quirks::eChipType::SCHIP;

    return Quirks::eChipType::CHIP8;
}

