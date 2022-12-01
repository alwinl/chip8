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

void Display::clear_screen()
{
	for( uint16_t idx = 0; idx < sizeof(display_buffer); ++ idx )
		display_buffer[idx] = 0;
}

void Display::draw()
{
	uint16_t total_pixels = sizeof(display_buffer) * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++ pixel )
		res.draw_pixel( pixel % 64, pixel / 64, (display_buffer[pixel / 8] >> (pixel%8)) & 0x01 );

	res.repaint();
}

bool Display::set_pixels( uint8_t x, uint8_t y, uint8_t * buffer, uint8_t length )
{
	bool turned_off = false;

	for( uint16_t byte_offset = 0; byte_offset < length; ++byte_offset ) {
		turned_off |= process_byte( x, y, buffer[byte_offset] );
		y = (y + 1 ) % 32;
	}

	return turned_off;
}

bool Display::process_byte( uint8_t x, uint8_t y, uint8_t byte_to_draw )
{
	bool turned_off = false;

	for( uint8_t bit_offset = 0; bit_offset < 8; ++bit_offset ) {
		if( is_bit_nonzero( byte_to_draw, bit_offset ) )
			toggle_pixel( x, y, bit_offset );
	}

	return turned_off;
}

bool Display::is_bit_nonzero( uint8_t byte_to_draw, uint8_t bit_offset )
{
	return byte_to_draw  & (1 << (7-bit_offset) );
}

bool Display::toggle_pixel( uint8_t x, uint8_t y, uint8_t bit_offset )
{
	uint8_t x_ = (x + bit_offset) % 64;

	uint16_t idx = (x_ + y * WIDTH) / 8;
	uint8_t offset = (x_ + y * WIDTH) % 8;

	bool turned_off = (display_buffer[idx] & (1 << offset));

	display_buffer[idx] ^= (1 << offset);

	return turned_off;
}

