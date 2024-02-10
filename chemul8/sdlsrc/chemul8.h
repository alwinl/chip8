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

#ifndef CHEMUL8_H
#define CHEMUL8_H

#include <cstdint>

enum class Quirks { RESET, MEMORY, DISP_WAIT, CLIPPING, SHIFTING, JUMPING };

class Chemul8
{
public:

	int run( int argc, char *argv[] );

    void clear_screen();
    bool toggle_a_pixel( uint8_t x, uint8_t y );
	bool is_key_pressed( uint8_t key_no );
	bool key_captured( uint8_t& key_no );
    void set_delay_timer( uint8_t value );
    void set_sound_timer( uint8_t value );
    uint8_t get_delay_timer( ) const;
    bool has_quirk( Quirks test_quirk );
	uint8_t get_random_value();
	bool block_drw();

private:
	static const uint8_t WIDTH = 64;
	static const uint8_t HEIGHT = 32;
    uint8_t display_buffer[ WIDTH * HEIGHT / 8 ];

	uint16_t keys = 0;
	uint16_t last_keys = 0;

	bool interrupt = false;

	uint8_t DelayTimer = 0;
	uint8_t SoundTimer = 0;

	struct QuirkFlags {
		bool reset_quirk;
		bool memory_quirk;
		bool display_wait_quirk;
		bool clipping_quirk;
		bool shifting_quirk;
		bool jumping_quirk;
	};

	QuirkFlags quirks;
};

#endif // CHEMUL8_H
