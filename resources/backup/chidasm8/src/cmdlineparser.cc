/*
 * filenameextractor.cc Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
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
    std::string fake_prog_name = "chidasm8";
    argv_ptrs.push_back(fake_prog_name.data());

    for (std::string& arg : arguments)
        argv_ptrs.push_back(arg.data());

    int argc = static_cast<int>(argv_ptrs.size());
    char** argv = argv_ptrs.data();

	return parse_args(argc, argv);
}

bool CmdLineParser::parse_args( int argc, char **argv )
{
	cxxopts::Options options("chidasm8", "Disassembler for CHIP-8 files" );

	options.add_options()
		( "h,help", "Show this help message" )
		( "v,verbose", "Verbose output" )
		( "c,clean", "Assembly only output" )
		( "p,program", "Program name", cxxopts::value<std::string>() )
		( "o,output", "Output file name", cxxopts::value<std::string>() )
		( "source", "Source file name", cxxopts::value<std::string>() );

	options.parse_positional({"source"});
	options.positional_help("<source>");
	options.show_positional_help();

	if( argc < 2 )
		throw std::runtime_error( options.help() + "\nError: Source file is required.\n" );

	try {
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::exception& e) {
		throw std::runtime_error( options.help() + "\nError: " + std::string(e.what()) );
	}

	if (result.count("help")) {
        std::cout << options.help() << std::endl;
		return false;
    }

    if( !result.count("source") )
		throw std::runtime_error( options.help() + "\nError: Source file is required.\n" );

	return true;
}

std::string CmdLineParser::get_source_name()
{
	return result["source"].as<std::string>();
}

std::string CmdLineParser::get_program_name()
{
	return ( result.count("p") == 0 ) ? get_output_prefix() : result["program"].as<std::string>();
}

std::string CmdLineParser::get_output_name()
{
	if( result.count("o") != 0 )
		return result["o"].as<std::string>();

	if( result.count("clean") != 0 )
		return get_output_prefix() + ".asm";

	return get_output_prefix() + ".lst";
}

bool CmdLineParser::is_verbose() const
{
    return result["verbose"].as<bool>();
}

bool CmdLineParser::is_clean() const
{
    return result["clean"].as<bool>();
}

uint16_t CmdLineParser::get_origin() const
{
	return 0x200;
}

bool CmdLineParser::show_help() const
{
    return result.count("help") > 0;
}

const std::string CmdLineParser::get_output_prefix()
{
	std::string source = get_source_name();
    std::filesystem::path path(source);
    return path.stem().string();
}
