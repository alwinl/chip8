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

#include "keyboard.h"
#include "chip8.h"

#include "resourcelayer.h"

bool Keyboard::is_key_pressed( int key_no )
{
	keys = res.check_key_event();

	return (keys >> key_no) & 0x01;
}

void Keyboard::wait_for_key()
{
	waiting_on_key = true;
}

bool Keyboard::executing() const
{
	return !waiting_on_key;
}

void Keyboard::check_key_captured()
{
	if( !waiting_on_key )
		return;

	keys = res.check_key_event();

	uint16_t key_changes = keys ^ last_keys;

	if( ! key_changes )
		return;

	last_keys = keys;

	for( int key_no = 0; key_no < 16; ++key_no )
		if( (key_changes >> key_no) & 0x01 ) {
			if( (keys >> key_no) & 0x01 )  {
				process_key( key_no );
				waiting_on_key = false;
				return;
			}
		}
}

void Keyboard::process_key( uint8_t key_value )
{
	for_each( subscribers.begin(), subscribers.end(),
		[key_value]( std::reference_wrapper<KeyTrigger>& subscriber )
		{
			subscriber.get().key_captured( key_value );
		}
	);
}

KeyTrigger::KeyTrigger( Keyboard& keyboard )
{
	keyboard.add_subcriber( *this );
}

void Keyboard::add_subcriber( KeyTrigger& new_subscriber )
{
	subscribers.push_back( new_subscriber );
}
