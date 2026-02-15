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
#include "assembler/cmdlineparser.h"
#include "assembler/assembler.h"
#include "assembler/loader.h"

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

		auto [ir, symbols] = assembler.build_ir( source );

		BinImage bin_image = BinaryEncoder().encode(ir);

		std::ofstream os( args.get_binary_name(), std::ios::binary );
		save_binary( os, bin_image );

		if( !args.get_listing_name().empty() ) {
			std::ofstream listing_os( args.get_listing_name() );
			ASMEmitter().emit( listing_os, ir, bin_image, symbols, ASMEmitter::OutputMode::Listing  );
		}

	}

	catch( const std::exception &e ) {
		std::cerr << e.what() << std::endl;
        return 2;
	}

	return 0;
}
