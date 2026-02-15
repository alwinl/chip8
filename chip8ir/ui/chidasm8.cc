/*
 * chidasm8.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "disassembler/cmdlineparser.h"
#include "disassembler/disassembler.h"
#include "ir/binary_loader.h"
#include "assembler/emitter.h"
#include "ir/cfg_emitter.h"

int main( int argc, char ** argv )
{
    try {

        ChidasmCmdLineParser args;

        if( !args.parse_args( argc, argv ) )
			return 1;

		Disassembler disassembler;
		disassembler.configure( {args.get_origin()} );

		std::ifstream is( args.get_source_name(), std::ios::binary );
		BinImage image = load_binary( is );

		auto [ir, symbols] = disassembler.build_ir( image );

		std::ofstream os( args.get_output_name() );
		ASMEmitter emitter;

		emitter.configure( {args.get_program_name()} );
		emitter.emit( os, ir, image, symbols, args.is_clean() ? ASMEmitter::OutputMode::Assembly : ASMEmitter::OutputMode::Listing );

		if( !args.get_dot_name().empty() || !args.get_uml_name().empty() ) {
			CFGEmitter cfg_emitter;

			if( !args.get_uml_name().empty() ) {
				std::ofstream plantuml_file(args.get_uml_name());
				cfg_emitter.emit(plantuml_file, ir, CFGEmitter::OutputMode::PlantUML);
				plantuml_file.close();
			}

			if( !args.get_dot_name().empty() ) {
				std::ofstream dot_file(args.get_dot_name());
				cfg_emitter.emit(dot_file, ir, CFGEmitter::OutputMode::Dot);
				dot_file.close();
			}
		}

    }

	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }

	return 0;
}
