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
#include <random>

#include "chemul8.h"

#include "resourcelayer.h"

#include "chip8.h"

//#define IS_SCHIP
//#define IS_XO_CHIP

int Chemul8::run( int argc, char *argv[] )
{
	if( argc < 2 ) {
		std::cout << "Usage chemul8 [program binary]" << std::endl;
		return 1;
	}

	ResourceLayer SDLRef;
	auto last_time = std::chrono::system_clock::now();
	ResourceLayer::Events event = ResourceLayer::Events::RESTART_EVENT;

#ifdef IS_SCHIP
	quirks = {
		.reset_quirk = false,
		.memory_quirk = false,
		.display_wait_quirk = false,
		.clipping_quirk = true,
		.shifting_quirk = true,
		.jumping_quirk = true,
	};
#else
 #ifdef IS_XO_CHIP
	quirks = {
		.reset_quirk = false,
		.memory_quirk = true,
		.display_wait_quirk = false,
		.clipping_quirk = false,
		.shifting_quirk = false,
		.jumping_quirk = false,
	};
 #else
	quirks = {
		.reset_quirk = true,
		.memory_quirk = true,
		.display_wait_quirk = true,
		.clipping_quirk = true,
		.shifting_quirk = false,
		.jumping_quirk = false,
	};
 #endif
#endif

	Chip8 device( *this );

	while( event != ResourceLayer::Events::QUIT_EVENT ) {

		if( event == ResourceLayer::Events::RESTART_EVENT ) {
			std::ifstream is = std::ifstream( argv[1] );
			if( !is.good() ) {
				std::cout << "Cannot read program" << std::endl;
				return 1;
			}

			//device.load_program( is );
			uint8_t buffer[4096];
			uint16_t address;

			uint8_t ch =  is.get();

			for( address = 0; is.good(); ++address ) {
				buffer[address] = ch;
				ch = is.get();
			}
			device.load_program( buffer, address );

		}

		if( SoundTimer != 0 )
			SDLRef.make_sound();

		/* this bit is to rate limit the DRW call to 60fps and do proper timing */
		interrupt = ( std::chrono::duration<double,std::milli>( std::chrono::system_clock::now() - last_time ).count() > 16 );

		if( interrupt ) {

			last_time = std::chrono::system_clock::now();

			if( DelayTimer )
				--DelayTimer;

			if( SoundTimer )
				--SoundTimer;
		}

		SDLRef.draw_buffer( display_buffer, sizeof(display_buffer) * 8 );

		event = SDLRef.check_events( keys );

		device.execute_instruction( );
	}

	return 0;
}

void Chemul8::clear_screen()
{
	for( uint16_t idx = 0; idx < sizeof(display_buffer); ++ idx )
		display_buffer[idx] = 0;
}

bool Chemul8::toggle_a_pixel( uint8_t x, uint8_t y )
{
	uint16_t idx = (x + y * WIDTH) / 8;
	uint8_t offset = (x + y * WIDTH) % 8;

	bool turned_off = (display_buffer[idx] & (1 << offset));

	display_buffer[idx] ^= (1 << offset);

	return turned_off;
}

bool Chemul8::key_captured( uint8_t& key_no )
{
	uint16_t key_changes = keys ^ last_keys;

	if( ! key_changes )
		return false;

	last_keys = keys;

	for( key_no = 0; key_no < 16; ++key_no )
		if( ( (key_changes >> key_no) & 0x01 ) && ( !( (keys >> key_no) & 0x01 ) ) )
			return true;

	return false;
}

bool Chemul8::is_key_pressed( uint8_t key_no )
{
	return (keys >> key_no) & 0x01;
}

void Chemul8::set_delay_timer( uint8_t value )
{
	DelayTimer = value;
}

void Chemul8::set_sound_timer( uint8_t value )
{
	SoundTimer = value;
}

uint8_t Chemul8::get_delay_timer( ) const
{
	return DelayTimer;
}

bool Chemul8::has_quirk( Quirks test_quirk )
{
	switch( test_quirk ) {
	case Quirks::RESET: return quirks.reset_quirk;
	case Quirks::MEMORY: return quirks.memory_quirk;
	case Quirks::DISP_WAIT: return quirks.display_wait_quirk;
	case Quirks::CLIPPING: return quirks.clipping_quirk;
	case Quirks::SHIFTING: return quirks.shifting_quirk;
	case Quirks::JUMPING: return quirks.jumping_quirk;
	};

	return false;
}

uint8_t Chemul8::get_random_value()
{
	static std::mt19937 mt{ std::random_device{}() };

	std::uniform_int_distribution<> dist(1,255);
	return dist( mt );
}

bool Chemul8::block_drw()
{
	return quirks.display_wait_quirk && !interrupt;
}

