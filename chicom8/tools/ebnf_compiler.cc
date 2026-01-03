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

#include "ebnf_tokeniser.h"
#include "ebnf_parser.h"
#include "ebnf_printer.h"
#include "ebnf_transformer.h"
#include "ebnf_generator.h"

int main()
{
    std::string base = "/home/alwin/Documents/Programming/CHIP8/chicom8/tools/ebnf";
    // std::string base = "/home/alwin/Documents/Programming/CHIP8/chicom8/src/chicom8";

    std::filesystem::path source_file( base + ".bnf");
    std::filesystem::path grammar_file( base +".grammar");
    std::filesystem::path ast_header( base +"_ast_test.h");

    Tokens tokens = Tokeniser( source_file ).tokenise_all();

    SyntaxTree grammar = Parser( tokens ).parse_all();

    // Transformer transformer = Transformer( grammar );
    // GrammarIR ir = transformer.transform_all();

    // std::cout << "Printing the sccs's:\n--------------------\n";
    // transformer.print_all_cycles( std::cout );
    // std::cout << "\n";

    // std::cout << "\nTransformer output:\n--------------------\n";
    // std::cout << transformer;
    // std::cout << "\n";

    Generator generator( grammar );
    generator.create_svg_image( base );
    generator.create_grammar_file( grammar_file );

    generator.create_ast_header( ast_header, "ebnf_tokeniser.h" );

    std::cout << "\nGraph output:\n--------------------\n";
    generator.stream_graph( std::cout );
    std::cout << "\n";

    return 0;
}
