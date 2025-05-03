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

	ResourceLayer();
	virtual ~ResourceLayer();

	void make_sound();
	void draw_buffer( uint8_t *buffer, uint16_t size );
	void check_events( uint16_t &keys );

	bool should_quit() const { return last_event == Events::QUIT_EVENT; }
	bool should_restart() const { return last_event == Events::RESTART_EVENT; }
private:
	enum class Events { NO_EVENT, QUIT_EVENT, RESTART_EVENT };

	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	Events last_event = Events::RESTART_EVENT;

	bool switch_event( SDL_Event &event, uint16_t &keys, ResourceLayer::Events &the_event );
	void draw_pixel( uint8_t x_pos, uint8_t y_pos, bool white );
};
