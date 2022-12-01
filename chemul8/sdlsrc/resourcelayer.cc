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

#include "resourcelayer.h"

#include <SDL.h>

#include <map>
#include <iostream>

InitError::InitError() : exception(), msg( SDL_GetError() )
	{}

InitError::InitError( const std::string & m ) : exception(), msg( m )
	{}

const char * InitError::what() const noexcept
{
	return msg.c_str();
}


std::map<SDL_Keycode,uint8_t> mapping = {
	{SDLK_0, 0x00},	{SDLK_1, 0x01},	{SDLK_2, 0x02},	{SDLK_3, 0x03},
	{SDLK_4, 0x04},	{SDLK_5, 0x05},	{SDLK_6, 0x06},	{SDLK_7, 0x07},
	{SDLK_8, 0x08},	{SDLK_9, 0x09},	{SDLK_a, 0x0A},	{SDLK_b, 0x0B},
	{SDLK_c, 0x0C},	{SDLK_d, 0x0D},	{SDLK_e, 0x0E},	{SDLK_f, 0x0F}
};



ResourceLayer::ResourceLayer()
{
	if( SDL_Init(  SDL_INIT_VIDEO | SDL_INIT_TIMER  ) != 0 )
		throw InitError();

	if( SDL_CreateWindowAndRenderer( 640, 320, SDL_WINDOW_SHOWN, &m_window, &m_renderer ) != 0 )
		throw InitError();

}

ResourceLayer::~ResourceLayer()
{
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );

    SDL_Quit();
}

uint16_t ResourceLayer::check_key_event()
{
	SDL_Event event;

	while( SDL_PollEvent( &event ) ) {
		try{
			switch( event.type ) {
			case SDL_KEYUP: {
					uint16_t mask = (1 << mapping.at(event.key.keysym.sym) );

					if( (keys & mask) != 0 ) {
						keys &= ~mask;
						return keys;
					}
				}
				break;

			case SDL_KEYDOWN: {
					uint16_t mask = (1 << mapping.at(event.key.keysym.sym) );

					if( (keys & mask ) == 0 ) {
						keys |= mask;
						return keys;
					}
				}
				break;

			case SDL_QUIT:
				quit = 1;
				break;
			}
		}
		catch( std::out_of_range& e ) {
			// ignore keys we are not interested in
		}
	}

	return keys;
}

void ResourceLayer::draw_pixel( uint8_t x_pos, uint8_t y_pos, bool white )
{
	SDL_Rect pixel_loc = { .x = (x_pos * 10), .y = (y_pos * 10), .w = 10, .h = 10 };

	if( white )
		SDL_SetRenderDrawColor( m_renderer, 255, 255, 255, 255 );	// white
	else
		SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );	// black

	SDL_RenderFillRect( m_renderer, &pixel_loc );
}

void ResourceLayer::repaint()
{
    SDL_RenderPresent( m_renderer );
}

bool ResourceLayer::frame_time()
{
	if( SDL_GetTicks64() <  mark_time + 17 )							// running at 60 FPS which is 16.6 ms
		return false;

	mark_time = SDL_GetTicks64();
	return true;
}

void ResourceLayer::make_sound()
{
	if( audio_on)
		std::cout << '\a';
}
