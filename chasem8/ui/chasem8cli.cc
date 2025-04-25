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

#include <fstream>
#include <iostream>

#include "commandlineparser.h"
#include "assembler.h"

int main( int argc, char **argv )
{
	if( argc < 2 ) {
		std::cout << "Usage: chasem8 source [binary [listing] ]" << std::endl;
		return 1;
	}

	CommandLineParser args( argc, argv );

	std::ifstream source = std::ifstream( args.get_source_name() );
	if( source.fail() ) {
		std::cout << "Cannot open source file" << std::endl;
		return 2;
	}
	
	if( source.peek() == std::ifstream::traits_type::eof() ) {
		std::cout << "Source file is empty" << std::endl;
		return 2;
	}

	std::ofstream binary = std::ofstream( args.get_binary_name() );
	if( binary.bad() ) {
		std::cout << "Cannot open output file" << std::endl;
		return 3;
	}

	std::ofstream listing = std::ofstream( args.get_listing_name() );
	if( listing.bad() )
		std::cout << "Cannot open listing file. Listing is not generated" << std::endl;

	Assembler worker;
	worker.read_source( source );
	worker.write_binary( binary );

	if( ! listing.bad() )
		worker.write_listing( listing );

	return 0;
}