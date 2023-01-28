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

void Keyboard::set_keys_state( uint16_t new_keys )
{
	keys = new_keys;
}

bool Keyboard::is_key_pressed( int key_no )
{
	return (keys >> key_no) & 0x01;
}

uint8_t Keyboard::which_key_captured( )
{
	uint16_t key_changes = keys ^ last_keys;

	if( ! key_changes )
		return (uint8_t) -1;

	last_keys = keys;

	for( int key_no = 0; key_no < 16; ++key_no )
		if( ( (key_changes >> key_no) & 0x01 ) && ( !( (keys >> key_no) & 0x01 ) ) )
			return key_no;

	return (uint8_t) -1;
}

