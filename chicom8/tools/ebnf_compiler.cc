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
#include "ebnf_generator.h"

int main()
{
    std::filesystem::path source_file( "/home/alwin/Documents/Programming/CHIP8/chicom8/src/chicom8.bnf");
    std::filesystem::path grammar_file( "/home/alwin/Documents/Programming/CHIP8/chicom8/src/chicom8.grammar");

    Tokens tokens = Tokeniser( source_file ).tokenise_all();
    Grammar grammar = Parser( tokens ).syntax_tree();


    std::ofstream output( grammar_file );
    output << grammar;
    // Generator().generateAstFiles( std::cout, grammar);
    
    return 0;
}
