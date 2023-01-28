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

#include <iostream>
#include <fstream>
#include <chrono>

#include "chemul8.h"

#include "resourcelayer.h"
#include "display.h"
#include "keyboard.h"
#include "timers.h"

#include "chip8.h"

using namespace std::chrono;

int Chemul8::run( int argc, char *argv[] )
{
	if( argc < 2 ) {
		std::cout << "Usage chemul8 [program binary]" << std::endl;
		return 1;
	}

	ResourceLayer SDLRef;

	Display display;
	Keyboard keyboard;
	Timers timers;

	Chip8 device( display, keyboard, timers );

	std::ifstream is = std::ifstream( argv[1] );
	if( !is.good() ) {
		std::cout << "Cannot read program" << std::endl;
		return 1;
	}

	device.load_program( is );

	auto last_time = system_clock::now();
	bool interrupt = false;

	while( !SDLRef.do_quit() ) {

		if( duration<double,std::milli>(system_clock::now() - last_time ).count() > 16 ) {
			interrupt = true;
			device.set_int_state( true );
			last_time = system_clock::now();
		}

		SDLRef.make_sound();

		if( interrupt )
			timers.decrease_timers( SDLRef );

		display.draw( SDLRef );

		uint16_t keys = SDLRef.check_events();

		keyboard.set_keys_state( keys );

		device.execute_instruction();

		if( interrupt ) {
			interrupt = false;
			device.set_int_state( false );
		}
	}


	return 0;
}



