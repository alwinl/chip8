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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>

class ResourceLayer;

class Display
{
public:
    Display();

    void clear_screen();
    bool set_pixels( uint8_t x, uint8_t y, uint8_t * buffer, uint8_t length );
    void draw( ResourceLayer& res );

    bool toggle_a_pixel( uint8_t x, uint8_t y );

private:
	static const uint8_t WIDTH = 64;
	static const uint8_t HEIGHT = 32;

    uint8_t display_buffer[ WIDTH * HEIGHT / 8 ];

	bool process_byte( uint8_t x, uint8_t y, uint8_t byte_to_draw );
	bool is_bit_nonzero( uint8_t byte_to_draw, uint8_t bit_offset );
	bool toggle_pixel( uint8_t x, uint8_t y, uint8_t bit_offset );
};

#endif // DISPLAY_H
