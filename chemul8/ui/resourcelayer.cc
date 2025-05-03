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

#include <SDL3/SDL.h>

#include <map>
#include <stdexcept>

std::map<SDL_Keycode, uint8_t> mapping = { { SDLK_0, 0x00 }, { SDLK_1, 0x01 }, { SDLK_2, 0x02 }, { SDLK_3, 0x03 },
										   { SDLK_4, 0x04 }, { SDLK_5, 0x05 }, { SDLK_6, 0x06 }, { SDLK_7, 0x07 },
										   { SDLK_8, 0x08 }, { SDLK_9, 0x09 }, { SDLK_A, 0x0A }, { SDLK_B, 0x0B },
										   { SDLK_C, 0x0C }, { SDLK_D, 0x0D }, { SDLK_E, 0x0E }, { SDLK_F, 0x0F } };

/*
SDL_AudioDeviceID audio_id;

void AudioCallback( void * userdata, Uint8* stream, int len )
{

}
*/
ResourceLayer::ResourceLayer()
{
	if( ! SDL_Init( SDL_INIT_VIDEO ) )
		throw std::runtime_error( SDL_GetError() );

	if( ! SDL_CreateWindowAndRenderer( "Chemul8", 640, 320, 0, &m_window, &m_renderer ) != 0 )
		throw std::runtime_error( SDL_GetError() );

	/*
		SDL_AudioSpec desired = {
			.freq = 48000,
			.format = AUDIO_F32,
			.channels = 2,
			.samples = 4096,
			.callback = AudioCallback;  // you wrote this function elsewhere.
		};

		SDL_AudioSpec obtained = {};

		audio_id = SDL_OpenAudioDevice( NULL, 0, &desired, &obtained, 0 );
		if( audio_id == 0 )
			throw std::runtime_error( SDL_GetError() );
	*/
}

ResourceLayer::~ResourceLayer()
{
	// SDL_CloseAudioDevice( audio_id );

	SDL_DestroyWindow( m_window );
	SDL_DestroyRenderer( m_renderer );

	SDL_Quit();
}

void ResourceLayer::check_events( uint16_t &keys )
{
	SDL_Event event;
	Events the_event = Events::NO_EVENT;

	while( SDL_PollEvent( &event ) ) {
		if( switch_event( event, keys, the_event ) )
			break;
	}

	last_event = the_event;
}

bool ResourceLayer::switch_event( SDL_Event &event, uint16_t &keys, ResourceLayer::Events &the_event )
{
	switch( event.type ) {
	case SDL_EVENT_KEY_UP:
		try {
			uint16_t mask = ( 1 << mapping.at( event.key.key ) );

			if( ( keys & mask ) != 0 ) {
				keys &= ~mask;
				return true;
			}
		} catch( std::out_of_range &e ) {
			// ignore keys we are not interested in
		}
		break;

	case SDL_EVENT_KEY_DOWN:
		try {
			if( event.key.key == SDLK_ESCAPE ) {
				the_event = Events::QUIT_EVENT;
				return true;
			}
			if( event.key.key == SDLK_F1 ) {
				the_event = Events::RESTART_EVENT;
				return true;
			}

			uint16_t mask = ( 1 << mapping.at( event.key.key ) );

			if( ( keys & mask ) == 0 ) {
				keys |= mask;
				return true;
			}
		} catch( std::out_of_range &e ) {
			// ignore keys we are not interested in
		}
		break;

	case SDL_EVENT_QUIT:
		the_event = Events::QUIT_EVENT;
		return true;
	}

	return false;
}

void ResourceLayer::draw_pixel( uint8_t x_pos, uint8_t y_pos, bool white )
{
	const SDL_FRect pixel_loc = { .x = ( x_pos * 10.0F ), .y = ( y_pos * 10.0F ), .w = 10, .h = 10 };

	if( white )
		SDL_SetRenderDrawColor( m_renderer, 255, 255, 255, 255 ); // white
	else
		SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 ); // black

	SDL_RenderFillRect( m_renderer, &pixel_loc );
}

void ResourceLayer::draw_buffer( uint8_t *buffer, uint16_t total_pixels )
{
	uint16_t total_pixels = size * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++pixel )
		draw_pixel( pixel % 64, pixel / 64, ( buffer[pixel / 8] >> ( pixel % 8 ) ) & 0x01 );

	SDL_RenderPresent( m_renderer );
}

/// @TODO
void ResourceLayer::make_sound()
{
	// if( audio_on)
	//		std::cout << '\a';
}