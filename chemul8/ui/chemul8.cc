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
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>

#include "resourcelayer.h"
#include "quirks.h"
#include "chip8.h"
#include "cmdline_processor.h"
#include <cstring>

static constexpr uint16_t display_size = 64 * 32 / 8;
static constexpr uint16_t display_base = 0x0F00;
static constexpr uint8_t ST_index = 0x17;			// 1 byte
static constexpr uint8_t keys_index = 0x18;		// 2 bytes
static constexpr uint8_t int_index = 0x1C;			// 1 byte

int run( std::string program, Quirks::eChipType chip_type )
{
	uint8_t buffer[4096];
	ResourceLayer SDLRef;
	auto last_time = std::chrono::system_clock::now();

	std::memset( buffer, 0, sizeof( buffer ) );
	Chip8 device( chip_type, buffer );

	while( ! SDLRef.should_quit() ) {

		if( SDLRef.should_restart() ) {
			std::ifstream is = std::ifstream( program );
			if( !is.good() ) {
				std::cout << "Cannot read program" << std::endl;
				return 1;
			}

			std::memset( buffer, 0, sizeof( buffer ) );

			uint16_t address;

			uint8_t ch = is.get();

			for( address = 0x200; is.good(); ++address ) {
				buffer[address] = ch;
				ch = is.get();
			}
			device.set_memory( buffer );
		}

		// if( device.make_sound() )
		if( buffer[ST_index] > 0 )
			SDLRef.make_sound();

		/* this bit is to rate limit the DRW call to 60fps and do proper timing */
		bool interrupt =
			( std::chrono::duration<double, std::milli>( std::chrono::system_clock::now() - last_time ).count() > 16 );

		if( interrupt )
			last_time = std::chrono::system_clock::now();

		buffer[int_index] = interrupt ? 1 : 0;

		SDLRef.draw_buffer( &buffer[display_base], display_size );

		uint16_t keys = (buffer[keys_index] << 8 ) | buffer[keys_index + 1];

		SDLRef.check_events( keys );

		buffer[keys_index] = keys >> 8;
		buffer[keys_index + 1] = keys & 0xFF;


		device.execute_instruction();
	}

	return 0;
}

int main( int argc, char *argv[] )
{
	CmdlineProcessor cmd_line( argc, argv );

	if( cmd_line.get_program().empty() )
		return -1;

	return run( cmd_line.get_program(), cmd_line.get_chip_type() );
}
