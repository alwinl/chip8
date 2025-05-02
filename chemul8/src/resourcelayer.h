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
 */

#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

class ResourceLayer
{
public:
	enum class Events { NO_EVENT, QUIT_EVENT, RESTART_EVENT };

	ResourceLayer();
	virtual ~ResourceLayer();

	void make_sound();
	void draw_buffer( uint8_t *buffer, uint16_t total_pixels );
	Events check_events( uint16_t &keys );

private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;

	bool switch_event( SDL_Event &event, uint16_t &keys, ResourceLayer::Events &the_event );
	void draw_pixel( uint8_t x_pos, uint8_t y_pos, bool white );
};
