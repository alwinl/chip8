/*
 * chip8_compiler.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "chip8_compiler.h"
#include <fstream>
#include <iostream>

#include "tokeniser.h"
#include "parser.h"
#include "generator.h"

void Chip8Compiler::compile_file(const std::string& input_path, const std::string& output_path)
{
    std::ofstream outfile(output_path, std::ios::binary);
    if (!outfile) throw std::runtime_error("Failed to open output file");

    Tokeniser tokeniser( input_path );

    Tokens const tokens = tokeniser.tokenise_all();

    Parser parser;

    Program const program = parser.make_AST( tokens );

	// Generator const generator( program );
    // Just a stub for now
    outfile.put(0x00); // e.g., CLS opcode
}


void checkEntryPoint( const std::vector<Decl*>& decls )
{
    bool foundMain = false;

    for (auto *decl : decls) {
        if (auto *func = dynamic_cast<FuncDecl*>(decl)) {
            if( *func->identifier == Identifier("main") ) {
                if (foundMain)
                    throw std::runtime_error("Multiple definitions of 'main'");
                foundMain = true;
            }
        }
    }

    if (!foundMain)
        throw std::runtime_error("Program must define a function 'main'");
}
