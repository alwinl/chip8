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

#include <chrono>
#include <thread>

#include "resourcelayer.h"

Display::Display()
{
	clear_screen();
}

void Display::clear_screen()
{
	for( uint16_t idx = 0; idx < sizeof(display_buffer); ++ idx )
		display_buffer[idx] = 0;
}

void Display::draw( ResourceLayer& res )
{
	uint16_t total_pixels = sizeof(display_buffer) * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++ pixel )
		res.draw_pixel( pixel % 64, pixel / 64, (display_buffer[pixel / 8] >> (pixel%8)) & 0x01 );

	res.repaint();
}

bool Display::toggle_a_pixel( uint8_t x, uint8_t y )
{
	uint16_t idx = (x + y * WIDTH) / 8;
	uint8_t offset = (x + y * WIDTH) % 8;

	bool turned_off = (display_buffer[idx] & (1 << offset));

	display_buffer[idx] ^= (1 << offset);

	return turned_off;
}
