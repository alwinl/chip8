/*
 * generator.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdint>

#include "parser.h"



class Generator
{
public:
    Generator() = default;
    // Generator( const std::string& source ) : program  {}
    // Generator( std::filesystem::path file_path ) : tokens( Tokeniser( file_path ).tokenise_all() ) {}

	std::string emit_assembly( std::string source );
    // void evaluate();

    // void write_to_file( std::string file_name );

private:
	void emit_function( const FuncDecl& func, std::ostream& out );
	bool emit_stmt( const Stmt& stmt, std::ostream& out );
    // std::vector<uint8_t> binary;

    // const Program& program;

    // void setup();
    // void emit_startup();

    // void emitFunction( FuncDecl* func );

	// void getEntryPoint();

};