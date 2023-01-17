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

#include <vector>
#include <functional>

class KeyTrigger;

class Keyboard
{
public:
	bool is_key_pressed( int key_no );
	void wait_for_key( );
	bool executing() const;
	void check_key_captured( uint16_t new_keys );
	void add_subcriber( KeyTrigger& new_subscriber ) { subscribers.push_back( new_subscriber ); }

private:
	uint16_t keys = 0;
	uint16_t last_keys = 0;
	bool waiting_on_key = false;
	std::vector<std::reference_wrapper<KeyTrigger>> subscribers;

	void process_key( uint8_t key_value );
};

class KeyTrigger
{
public:
	KeyTrigger( Keyboard& keyboard_ ) : keyboard(keyboard_) { keyboard_.add_subcriber( *this ); }

	virtual void key_captured( uint8_t key_value ) = 0;

	bool is_key_pressed( int key_no ) { return keyboard.is_key_pressed( key_no ); };


protected:
	virtual ~KeyTrigger() {};
	void start_waiting() {
		keyboard.wait_for_key( );
	}

	Keyboard& keyboard;
};

#endif // KEYBOARD_H
