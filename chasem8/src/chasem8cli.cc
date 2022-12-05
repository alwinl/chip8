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

#include "chasem8cli.h"

#include "instruction.h"
#include "program.h"

#include <iostream>
#include <fstream>

#include "filenameextractor.h"

int Chasem8CLI::run( int argc, char ** argv )
{
	if( argc < 2 ) {
		std::cout << "Usage chasem8 source [binary [listing] ]" << std::endl;
		return 1;
	}

    FilenameExtractor filenames( argc, argv );

	std::ifstream source = std::ifstream( filenames.get_source_name().c_str() );
	if( source.bad() ) {
		std::cout << "Cannot open source file" << std::endl;
		return 2;
	}

	std::ofstream binary = std::ofstream( filenames.get_binary_name().c_str() );
	if( binary.bad() ) {
		std::cout << "Cannot open output file" << std::endl;
		return 3;
	}

	bool no_listing = false;
	std::ofstream listing = std::ofstream( filenames.get_listing_name().c_str() );
	if( listing.bad() ) {
		std::cout << "Cannot open lisintg file. Listing will  not be generated" << std::endl;
		no_listing = true;
	}


#if 0
	std::ifstream source = std::ifstream( argv[1] );
	if( !source.good() ) {
		std::cout << "Cannot read program" << std::endl;
		return 1;
	}

	Program prog;

	prog.read_source( source );

	std::ofstream binary_file = std
	prog.write_binary( )

	Instruction inst;

	prog.add_instruction( inst );
#endif // 0

	return 0;
}
