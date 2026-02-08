/*
 * chasm8.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "cmdlineparser.h"
#include "assembler.h"
#include "asm_loader.h"
#include "binary_emitter.h"
#include "listing_emitter.h"

int main( int argc, char ** argv )
{
	try {

		ChasmCmdLineParser args;

		if( ! args.parse_args( argc, argv ) )
			return 1;

		Assembler assembler;
		assembler.configure( args );

		std::ifstream asm_file( args.get_source_name() );
		ASMSource source = ASMSourceLoader().load( asm_file );

		IRProgram ir = assembler.build_ir( source );

		std::ofstream os( args.get_binary_name(), std::ios::binary );
		BinaryEmitter::emit( os, ir );

		if( !args.get_listing_name().empty() ) {
			std::ofstream listing_os( args.get_listing_name() );
			ListingEmitter::emit( listing_os, ir );
		}

	}

	catch( const std::exception &e ) {
		std::cerr << e.what() << std::endl;
        return 2;
	}

	return 0;
}
