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

#include "spritedrawer.h"

#include "resourcelayer.h"

void SpriteDrawer::read_data( std::istream &sprite_is )
{
	char chr = '\0';

	sprite_is.get( chr );

	if( !sprite_is.good() )
		return;

	sprite_height = chr;
	sprite_is.get( chr );

	while( sprite_is.good() ) {
		data.push_back( chr );
		sprite_is.get( chr );
	}
}

void SpriteDrawer::show()
{
	const uint8_t no_of_sprites = data.size() / sprite_height;
	ResourceLayer res = ResourceLayer( 8 * no_of_sprites, sprite_height );

	uint8_t y_pos = 0;
	uint8_t x_offset = 0;

	for( const uint8_t data_byte : data ) {
		for( int x_pos = 0; x_pos < 8; ++x_pos )
			res.draw_pixel( x_pos + x_offset, y_pos, ( ( data_byte & ( 1 << ( 8 - x_pos ) ) ) == 0 ) );

		++y_pos;

		if( y_pos == sprite_height ) {
			x_offset += 8;
			y_pos = 0;
		}
	}

	res.repaint();

	while( !res.do_quit() ) res.check_key_event();
}