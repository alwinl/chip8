/*
 * ebnf_compiler.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <iostream>
#include <filesystem>

#include <fstream>
#include <ostream>
#include <iostream>
#include <string>

#include "ebnf_tokeniser.h"
#include "ebnf_parser.h"
#include "ebnf_printer.h"
#include "ebnf_transformer.h"
#include "ebnf_generator.h"

#include "ebnf_cmdlineparser.h"

int main( int argc, char * argv[] )
{
	CmdLineParser args;

	if( ! args.parse_args(argc, argv) )
		return 1;

    std::filesystem::path input_file = args.get_input_name();

	Tokens tokens = Tokeniser( input_file ).tokenise_all();

	if( args.produce_token_list() ) {

		if( args.get_output_name() == "-" ) {
			for( auto token : tokens )
				std::cout << token << "\n";
		} else {
			std::filesystem::path output_file( args.get_output_name() + ".tokens" );
			std::ofstream os( output_file );
			if( !os )
				throw std::runtime_error("Cannot open: \"" + args.get_output_name() + "\"");
			for( auto token : tokens )
				os << token << "\n";
		}
	}

    SyntaxTree grammar = Parser( tokens ).parse_all();
    Generator generator( grammar );

	if( args.produce_grammar_json() ) {

		if( args.get_output_name() == "-" ) {
			generator.create_grammar_file( std::cout );
		} else {
			std::filesystem::path output_file( args.get_output_name() + ".json" );
			std::ofstream os( output_file );
			if( !os )
				throw std::runtime_error("Cannot open: \"" + args.get_output_name() + "\"");

			generator.create_grammar_file( os );
		}
	}

	if( args.produce_svg() ) {

		std::string dot_file_name = args.get_input_name() + ".dot";
		if( args.get_output_name() != "-" )
			dot_file_name = args.get_output_name() + ".dot";

		std::filesystem::path dot_file( dot_file_name );

		std::ofstream os( dot_file );
		if( !os )
			throw std::runtime_error("Cannot open: \"" + dot_file_name + "\"");

		generator.create_dot_source( os );
		os.close();

		std::string cmd = "dot -Tsvg " + dot_file_name;

		if( args.get_output_name() != "-" )
			cmd += " -o " + args.get_output_name() + ".svg";

		if( std::system(cmd.c_str()) != 0 )
			throw std::runtime_error("Graphviz 'dot' command failed.");
	}

	if( args.produce_graph() ) {

		if( args.get_output_name() == "-" ) {
			generator.stream_graph( std::cout );
		} else {
			std::filesystem::path output_file( args.get_output_name() + ".graph" );
			std::ofstream os( output_file );
			if( !os )
				throw std::runtime_error("Cannot open: \"" + args.get_output_name() + ".graph" + "\"");

			generator.stream_graph( os );
		}
	}

	std::string ast_header_file = args.get_ast_header_name();

	if( !ast_header_file.empty() ) {

		if( ast_header_file == "-" ) {

			generator.create_ast_header( std::cout, "ast_header" );

		} else {

			std::ofstream os( ast_header_file );
			if( !os )
				throw std::runtime_error("Cannot open: " + ast_header_file);

			generator.create_ast_header( os, "ast_header" );
		}
	}

	std::string tarjan_file = args.get_tarjan_name();

	if( !tarjan_file.empty() ) {

		Transformer transformer = Transformer( grammar );
		GrammarIR ir = transformer.transform_all();

		if( tarjan_file == "-" ) {

			std::cout << "Printing the sccs's:\n--------------------\n";
			transformer.print_all_cycles( std::cout );
			std::cout << "\n";

			std::cout << "\nTransformer output:\n--------------------\n";
			std::cout << transformer;
			std::cout << "\n";

		} else {

			std::ofstream os( tarjan_file );
			if( !os )
				throw std::runtime_error("Cannot open: " + tarjan_file);

			os << "Printing the sccs's:\n--------------------\n";
			transformer.print_all_cycles( os );
			os << "\n";

			os << "\nTransformer output:\n--------------------\n";
			os << transformer;
			os << "\n";
		}
	}
    // std::string base = "/home/alwin/Documents/Programming/CHIP8/chicom8/tools/ebnf";
    // // std::string base = "/home/alwin/Documents/Programming/CHIP8/chicom8/src/chicom8";

    // std::filesystem::path source_file( base + ".bnf");
    // std::filesystem::path grammar_file( base +".grammar");
    // std::filesystem::path ast_header( base +"_ast_test.h");

    // Tokens tokens = Tokeniser( source_file ).tokenise_all();

    // SyntaxTree grammar = Parser( tokens ).parse_all();

    // Transformer transformer = Transformer( grammar );
    // GrammarIR ir = transformer.transform_all();

    // std::cout << "Printing the sccs's:\n--------------------\n";
    // transformer.print_all_cycles( std::cout );
    // std::cout << "\n";

    // std::cout << "\nTransformer output:\n--------------------\n";
    // std::cout << transformer;
    // std::cout << "\n";



    return 0;
}
