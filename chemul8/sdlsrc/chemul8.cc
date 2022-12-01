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

#include "chemul8.h"

#include "resourcelayer.h"
#include "chip8.h"

int Chemul8::run( int argc, char *argv[] )
{
	if( argc < 2 ) {
		std::cout << "Usage chemul8 [program binary]" << std::endl;
		return 1;
	}

	ResourceLayer SDLRef;

	Display display;
	Keyboard keyboard( SDLRef );
	Timers timers( SDLRef );
	Randometer rander;

	Chip8 device( display, keyboard, timers, rander );

	std::ifstream is = std::ifstream( argv[1] );
	if( !is.good() ) {
		std::cout << "Cannot read program" << std::endl;
		return 1;
	}

	device.load_program( is );


	while( !SDLRef.do_quit() ) {

		SDLRef.make_sound();

		if( SDLRef.frame_time() ) {
			timers.decrease_timers();
			display.draw( SDLRef );
		}

		if( keyboard.executing() )
			device.execute_instruction();
		else
			keyboard.check_key_captured( device );
	}


	return 0;
}



