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
#include <string>

#include "resourcelayer.h"
#include "chip8.h"
#include "cmdlineparser.h"

size_t load_program( std::string &program, uint8_t* buffer )
{
	size_t bytes_read = 0;
	std::ifstream is = std::ifstream( program );

	while( is.good() )
		buffer[bytes_read++] = is.get();

	return bytes_read;
}

int run( std::string program )
{
	uint8_t buffer[4096];
	ResourceLayer SDLRef;
	auto last_time = std::chrono::system_clock::now();
	Chip8 device;
	uint16_t keys_state = 0;

	while( ! SDLRef.should_quit() ) {

		if( SDLRef.should_restart() ) {

			std::memset( buffer, 0, sizeof( buffer ) );

			size_t read = load_program( program, buffer );
			if( ! read ) {
				std::cout << "Cannot read program" << std::endl;
				return 1;
			}

			device.set_program( buffer, read );
			device.set_quirk_type( Chip8::eQuirkType::CHIP8 );
		}

		if( SDLRef.set_new_type( ) ) {
			switch( SDLRef.get_new_type() ) {
			case 1: device.set_quirk_type( Chip8::eQuirkType::SCHIP ); break;
			case 2: device.set_quirk_type( Chip8::eQuirkType::XOCHIP ); break;
			default: device.set_quirk_type( Chip8::eQuirkType::CHIP8 ); break;
			}
		}

		/* this bit is to rate limit the DRW call to 60fps and do proper timing */
		bool interrupt =
			( std::chrono::duration<double, std::milli>( std::chrono::system_clock::now() - last_time ).count() > 16 );

		if( interrupt ) {
			last_time = std::chrono::system_clock::now();
			device.decrease_timers();
		}

		device.set_interrupt( interrupt );

		SDLRef.check_events( keys_state );
		device.set_keys_state( keys_state );

		if( device.get_sound_active() )
			SDLRef.make_sound();

		SDLRef.draw_buffer( device.get_display_buffer(), device.get_display_size() );

		device.clock_tick();
	}

	return 0;
}

int main( int argc, char *argv[] )
{
	CmdLineParser cmd_line;

	cmd_line.parse_args( argc, argv );

	if( cmd_line.get_program().empty() )
		return -1;

	return run( cmd_line.get_program() );
}
