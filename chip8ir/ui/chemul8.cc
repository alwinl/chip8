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

#include <raylib.h>

#include "emulator/chip8.h"



void render_screen( uint8_t *buffer, uint16_t size)
{
	uint16_t total_pixels = size * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++pixel ) {

		Rectangle rect{ .x = 10.0F * (pixel % 64), .y = 10.0F * (pixel / 64), .width = 10.0F, .height = 10.0F };
		bool is_set = ( buffer[pixel / 8] >> ( pixel % 8 ) ) & 0x01;

		DrawRectangleRec( rect, is_set ? WHITE : BLACK );
	}
}

int main( int argc, char ** argv )
{
	Chip8 device;

    InitWindow(640, 320, "Chemul8");

    while( ! WindowShouldClose() ) {

        BeginDrawing();

        ClearBackground( BLACK );

		render_screen( device.get_display_buffer(), device.get_display_size() );

        EndDrawing();
    }

    CloseWindow();

}


// #include <iostream>
// #include <fstream>

// #include "ir/binary_loader.h"
// #include "ir/ir_bundle.h"

// #include "disassembler/disassembler.h"

// #include "emulator/emulator.h"

// int main( int argc, char ** argv )
// {
//     try {
//         ChemulCmdLineParser args;

//         if( !args.parse_args( argc, argv ) )
// 			return 1;

// 		Disassembler disassembler;
// 		disassembler.configure( {args.get_origin()} );

// 		std::ifstream is( args.get_program(), std::ios::binary );
// 		BinImage image = load_binary( is );

// 		IRBundle bundle = disassembler.build_ir( image );

//         Emulator emulator;
//         emulator.configure( args );

//         emulator.run( image, bundle.ir );
//     }

// 	catch (const std::exception &e) {
//         std::cerr << e.what() << std::endl;
//         return 2;
//     }

// 	return 0;
// }
