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

#ifndef RESOURCELAYER_H
#define RESOURCELAYER_H

#include <cstdint>
#include <cstdlib>

struct SDL_Window;
struct SDL_Renderer;

class ResourceLayer
{
public:
	ResourceLayer();
	virtual ~ResourceLayer();

	uint16_t check_key_event();
	void draw_pixel( uint8_t x_pos, uint8_t y_pos, bool white );
	void repaint();
	bool frame_time();
	void make_sound();

	bool do_quit() { return quit; }
	void audio( bool on ) { audio_on = on; };

private:
    SDL_Window * m_window;
    SDL_Renderer * m_renderer;

    uint64_t mark_time = 0;
    bool audio_on = 0;

	uint16_t keys = 0;
	bool quit = false;
};

#endif // RESOURCELAYER_H
