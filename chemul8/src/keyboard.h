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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>

class ResourceLayer;

class Keyboard
{
public:
	Keyboard( ResourceLayer& res_init ) : res( res_init) {};

	void check_key_event();
	bool is_key_pressed( int key_no ) { return (keys >> key_no ) & 0x01; };
	bool key_state_changed();
	uint8_t wait_for_key();

private:
	uint16_t keys;
	uint16_t last_keys;

	ResourceLayer& res;
};

#endif // KEYBOARD_H
