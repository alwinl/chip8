/*
 * chasem8cli.cc Copyright 2021 Alwin Leerling dna.leerling@gmail.com
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

#include "instruction.h"
#include "program.h"

#include <fstream>
#include <iostream>

#include "filenameextractor.h"

int main( int argc, char **argv )
{
	if( argc < 2 ) {
		std::cout << "Usage chasem8 source [binary [listing] ]" << std::endl;
		return 1;
	}

	Program prog;
	FilenameExtractor filenames( argc, argv );

	std::ifstream source = std::ifstream( filenames.get_source_name().c_str() );
	if( source.bad() ) {
		std::cout << "Cannot open source file" << std::endl;
		return 2;
	}

	prog.read_source( source );

	std::ofstream binary = std::ofstream( filenames.get_binary_name().c_str() );
	if( binary.bad() ) {
		std::cout << "Cannot open output file" << std::endl;
		return 3;
	}

	prog.write_binary( binary );

	std::ofstream listing = std::ofstream( filenames.get_listing_name().c_str() );
	if( ! listing.bad() )
		prog.write_listing( listing );
	else
		std::cout << "Cannot open lisintg file. Listing is not generated" << std::endl;

	return 0;
}