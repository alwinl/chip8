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

#include "chidisas8.h"

#include <iostream>
#include <fstream>

#include "disassembler.h"

int Chidisas8::run( int argc, char *argv[] )
{
	static const unsigned int start_address = 0x200;

	if( argc < 2 ) {
		std::cout << "Usage dis_chip8 [program binary]" << std::endl;
		return 1;
	}

	std::ofstream os = std::ofstream( std::string(argv[1]) + ".lst" );
	std::ifstream is = std::ifstream( argv[1] );

	if( ! is.good() || !os.good() ) {
		std::cout << "Cannot open files" << std::endl;
		return 1;
	}

	Disassembler dis( argv[1], start_address );
	dis.read_binary( is );
	dis.disassemble( );
	dis.collect_data_bytes();
	dis.write_listing( os );

    return 0;
}
