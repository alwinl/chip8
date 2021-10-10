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

#include "display.h"

#include "resourcelayer.h"

Display::Display( ResourceLayer& res_init ) : res( res_init )
{
	clear_screen();
}

void Display::draw()
{
	uint16_t total_pixels = sizeof(display_buffer) * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++ pixel )
		res.draw_pixel( pixel % 64, pixel / 64, (display_buffer[pixel / 8] >> (pixel%8)) & 0x01 );

	res.repaint();
}

void Display::clear_screen()
{
	for( uint16_t idx = 0; idx < sizeof(display_buffer); ++ idx )
		display_buffer[idx] = 0;

	draw();
}

/*
 * returns true if pixel was turned off
 */
bool Display::set_pixel( uint8_t x, uint8_t y, bool on )
{
	if( !on )
		return false;

	uint16_t idx = (x + y * WIDTH) / 8;
	uint8_t offset = (x + y * WIDTH) % 8;

	bool turned_off = (display_buffer[idx] & (1 << offset));

	display_buffer[idx] ^= (1 << offset);

	return turned_off;
}


bool Display::set_pixels( uint8_t x, uint8_t y, uint8_t * buffer, uint8_t length )
{
	bool turned_off = false;

	for( uint16_t buf_idx = 0; buf_idx < length; ++buf_idx ) {
		for( uint8_t offset = 0; offset < 8; ++offset ) {
			uint8_t x_ = x + offset;
			if( x_ >= 64 )
				x_ -= 64;

			turned_off |= set_pixel( x_, y, (buffer[buf_idx] >> (7-offset) ) & 1 );
		}

		if( ++y == 32 )
			y = 0;
	}

	draw();

	return turned_off;
}
