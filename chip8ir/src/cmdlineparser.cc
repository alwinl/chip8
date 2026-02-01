/*
 * cmdlineparser.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

bool ChasmCmdLineParser::parse_args( std::vector<std::string> arguments )
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

bool ChasmCmdLineParser::parse_args( int argc, char **argv )
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

std::string ChasmCmdLineParser::get_source_name()
{
	return result["source"].as<std::string>();
}

std::string ChasmCmdLineParser::get_binary_name()
{
	return ( result.count("o") == 0 ) ? get_output_prefix() + ".ch8" : result["o"].as<std::string>();
}

std::string ChasmCmdLineParser::get_listing_name()
{
	if( result.count("listing") == 0 )
		return "";

	if( result["listing"].as<std::string>() == "__absent__" )
		return get_output_prefix() + ".ch8.lst";

	return result["listing"].as<std::string>();
}

bool ChasmCmdLineParser::is_verbose() const {
    return result["verbose"].as<bool>();
}

bool ChasmCmdLineParser::show_help() const {
    return result.count("help") > 0;
}

const std::string ChasmCmdLineParser::get_output_prefix()
{
	std::string source = get_source_name();
    std::filesystem::path path(source);
    return path.stem().string();
}

bool ChidasmCmdLineParser::parse_args( std::vector<std::string> arguments )
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

bool ChidasmCmdLineParser::parse_args( int argc, char **argv )
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

std::string ChidasmCmdLineParser::get_output_name()
{
	if( result.count("o") != 0 )
		return result["o"].as<std::string>();

	if( result.count("clean") != 0 )
		return get_output_prefix() + ".asm";

	return get_output_prefix() + ".lst";
}

const std::string ChidasmCmdLineParser::get_output_prefix()
{
	std::string source = get_source_name();
    std::filesystem::path path(source);
    return path.stem().string();
}

std::string ChidasmCmdLineParser::get_source_name() { return result["source"].as<std::string>(); }
std::string ChidasmCmdLineParser::get_program_name() { return ( result.count("p") == 0 ) ? get_output_prefix() : result["program"].as<std::string>(); }
bool ChidasmCmdLineParser::is_verbose() const { return result["verbose"].as<bool>(); }
bool ChidasmCmdLineParser::is_clean() const { return result["clean"].as<bool>(); }
uint16_t ChidasmCmdLineParser::get_origin() const { return 0x200; }
bool ChidasmCmdLineParser::show_help() const { return result.count("help") > 0; }


bool ChemulCmdLineParser::parse_args( std::vector<std::string> arguments )
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


bool ChemulCmdLineParser::parse_args( int argc, char ** argv )
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

std::string ChemulCmdLineParser::get_program()
{
	return result["romfile"].as<std::string>();
}

bool ChicomCmdLineParser::parse_args( std::vector<std::string> arguments )
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

bool ChicomCmdLineParser::parse_args(int argc, char** argv)
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

std::string ChicomCmdLineParser::get_input_name() const
{
    if (result.count("source") == 0U)
		return {};

	return result["source"].as<std::string>();
}

std::string ChicomCmdLineParser::get_output_name() const
{
	if( result.count("o") != 0 )
        return result["o"].as<std::string>();

    if (result.count("source") == 0U)
		return {};

	std::string const source = result["source"].as<std::string>();
    std::filesystem::path const path(source);
    return path.stem().string() + ".ch8";
}

bool ChicomCmdLineParser::is_verbose() const
{
    return result["verbose"].as<bool>();
}

bool ChicomCmdLineParser::show_help() const
{
    return result.count("help") > 0;
}
