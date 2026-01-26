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

#include <algorithm>
#include <span>
#include <stdexcept>
#include <string>
#include <filesystem>
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

bool CmdLineParser::parse_args( int argc, char **argv )
{
    cxxopts::Options options("chasm8", "Assembler for CHIP-8 files" );

	options.add_options()
		("l,listing", "Generate listing file (optional filename)", cxxopts::value<std::string>()->implicit_value("__absent__"))
		( "h,help", "Show this help message" )
		( "v,verbose", "Verbose output" )
		( "o", "Output binary file name", cxxopts::value<std::string>() )
		( "source", "Source file name", cxxopts::value<std::string>() );

	options.parse_positional({"source"});
	options.positional_help("<source>");
	options.show_positional_help();

	try {
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::exception& e) {
		std::cerr << "Error parsing command line options: " << e.what() << std::endl;
		std::cerr << options.help() << std::endl;
		return false;
	}

	if (result.count("help")) {
        std::cout << options.help() << std::endl;
		return false;
    }

    if (!result.count("source")) {
		std::cerr << "Error: Source file is required." << std::endl;
		std::cerr << options.help() << std::endl;
		return false;
	}

	return true;
}

std::string CmdLineParser::get_source_name()
{
	return result["source"].as<std::string>();
}

std::string CmdLineParser::get_binary_name()
{
	return ( result.count("o") == 0 ) ? get_output_prefix() + ".ch8" : result["o"].as<std::string>();
}

std::string CmdLineParser::get_listing_name()
{
	if( result.count("listing") == 0 )
		return "";

	if( result["listing"].as<std::string>() == "__absent__" )
		return get_output_prefix() + ".ch8.lst";

	return result["listing"].as<std::string>();
}

bool CmdLineParser::is_verbose() const {
    return result["verbose"].as<bool>();
}

bool CmdLineParser::show_help() const {
    return result.count("help") > 0;
}

const std::string CmdLineParser::get_output_prefix()
{
	std::string source = get_source_name();
    std::filesystem::path path(source);
    return path.stem().string();
}
