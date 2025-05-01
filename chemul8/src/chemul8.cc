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

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>

#include "chemul8.h"
#include "resourcelayer.h"
#include "quirks.h"
#include "chip8.h"
#include "cmdline_processor.h"

int main( int argc, char *argv[] )
{
	CmdlineProcessor cmd_line( argc, argv );

	if( cmd_line.get_program().empty() )
		return -1;

	return Chemul8().run( cmd_line.get_program(), cmd_line.get_chip_type() );
}

int Chemul8::run( std::string program, Quirks::eChipType chip_type )
{
	ResourceLayer SDLRef;
	auto last_time = std::chrono::system_clock::now();
	ResourceLayer::Events event = ResourceLayer::Events::RESTART_EVENT;

	Chip8 device( *this, chip_type );
	uint8_t buffer[4096];

	while( event != ResourceLayer::Events::QUIT_EVENT ) {

		if( event == ResourceLayer::Events::RESTART_EVENT ) {
			std::ifstream is = std::ifstream( program );
			if( !is.good() ) {
				std::cout << "Cannot read program" << std::endl;
				return 1;
			}

			uint16_t address;

			uint8_t ch = is.get();

			for( address = 0x200; is.good(); ++address ) {
				buffer[address] = ch;
				ch = is.get();
			}

			// device.load_program( buffer, address );
			device.set_memory( buffer );
		}

		if( SoundTimer != 0 )
			SDLRef.make_sound();

		/* this bit is to rate limit the DRW call to 60fps and do proper timing */
		interrupt =
			( std::chrono::duration<double, std::milli>( std::chrono::system_clock::now() - last_time ).count() > 16 );

		if( interrupt ) {

			last_time = std::chrono::system_clock::now();

			if( DelayTimer )
				--DelayTimer;

			if( SoundTimer )
				--SoundTimer;
		}

		SDLRef.draw_buffer( display_buffer, sizeof( display_buffer ) * 8 );

		event = SDLRef.check_events( keys );

		device.execute_instruction( interrupt);
	}

	return 0;
}

void Chemul8::clear_screen()
{
	for( uint16_t idx = 0; idx < sizeof( display_buffer ); ++idx ) display_buffer[idx] = 0;
}

bool Chemul8::toggle_a_pixel( uint8_t x, uint8_t y )
{
	uint16_t idx = ( x + y * WIDTH ) / 8;
	uint8_t offset = ( x + y * WIDTH ) % 8;

	bool turned_off = ( display_buffer[idx] & ( 1 << offset ) );

	display_buffer[idx] ^= ( 1 << offset );

	return turned_off;
}

bool Chemul8::key_captured( uint8_t &key_no )
{
	uint16_t key_changes = keys ^ last_keys;

	if( !key_changes )
		return false;

	last_keys = keys;

	for( key_no = 0; key_no < 16; ++key_no )
		if( ( ( key_changes >> key_no ) & 0x01 ) && ( !( ( keys >> key_no ) & 0x01 ) ) )
			return true;

	return false;
}

bool Chemul8::is_key_pressed( uint8_t key_no )
{
	return ( keys >> key_no ) & 0x01;
}

void Chemul8::set_delay_timer( uint8_t value )
{
	DelayTimer = value;
}

void Chemul8::set_sound_timer( uint8_t value )
{
	SoundTimer = value;
}

uint8_t Chemul8::get_delay_timer() const
{
	return DelayTimer;
}

uint8_t Chemul8::get_random_value()
{
	static std::mt19937 mt{ std::random_device{}() };

	std::uniform_int_distribution<> dist( 1, 255 );
	return dist( mt );
}

