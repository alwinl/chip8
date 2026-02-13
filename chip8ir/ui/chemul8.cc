/*
 * chemul8.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include <fstream>

#include "emulator/cmdlineparser.h"
#include "disassembler/disassembler.h"
#include "ir/binary_loader.h"
#include "emulator/emulator.h"

int main( int argc, char ** argv )
{
    try {
        ChemulCmdLineParser args;

        if( !args.parse_args( argc, argv ) )
			return 1;

		Disassembler disassembler;

		std::ifstream is( args.get_program(), std::ios::binary );
		BinImage image = load_binary( is );

		IRProgram ir = disassembler.build_ir( image );

        Emulator emulator;
        emulator.configure( args );

        emulator.run( image, ir );
    }

	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }

	return 0;
}
