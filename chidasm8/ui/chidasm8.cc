/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
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
 *
 *
 */

#include <fstream>
#include <iostream>

#include "commandlineparser.h"
#include "disassembler.h"

int main( int argc, char *argv[] )
{
	static const unsigned int start_address = 0x200;

	CommandLineParser args( argc, argv );

	std::ifstream source_file = std::ifstream( args.get_source_name(), std::ios::in | std::ios::binary );
	if( source_file.fail() ) {
		std::cout << "Cannot open source file" << std::endl;
		return 2;
	}

	if( source_file.peek() == std::ifstream::traits_type::eof() ) {
		std::cout << "Source file is empty" << std::endl;
		return 2;
	}

	std::ofstream output_file = std::ofstream( args.get_output_name(), std::ios::out );
	if( !output_file.is_open() ) {
		std::cout << "Cannot open output file" << std::endl;
		return 3;
	}


	if( argc < 2 ) {
		std::cout << "Usage dis_chip8 [program binary]" << std::endl;
		return 1;
	}

	InputData input( args.get_program_name(), start_address );

	source_file >> input;

	Disassembler worker(input);

	worker.disassemble();

	worker.print_output( output_file );

	return 0;
}