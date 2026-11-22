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

#include "ir/binary_emitter.h"
#include "ir/asm_emitter.h"
#include "ir/encoder.h"
#include "ir/ir_bundle.h"

#include "assembler/cmdlineparser.h"
#include "assembler/assembler.h"

int main( int argc, char ** argv )
{
	try {
		assembler::CmdLineParser args;

		if( !args.parse_args( argc, argv ) )
			return 1;

		std::ifstream is( args.get_source_name() );
		const IRBundle bundle = assembler::build_ir( is, args.get_source_name() );

		std::ofstream os( args.get_binary_name(), std::ios::binary );
		binary_emitter::emit( os, bundle );

		if( !args.get_listing_name().empty() ) {
			std::ofstream listing_os( args.get_listing_name() );
			asm_emitter::emit( listing_os, bundle, args.get_source_name(), asm_emitter::OutputMode::Listing  );
		}
	}

	catch( const std::exception &e ) {
		std::cerr << e.what() << std::endl;
        return 2;
	}

	return 0;
}
