/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#ifndef TIMERS_H
#define TIMERS_H

#include <cstdint>

class ResourceLayer;

class Timers
{
public:
    Timers() {};

    void set_delay_timer( uint8_t value );
    uint8_t get_delay_timer( ) const;
    void set_sound_timer( uint8_t value );

    void decrease_timers( ResourceLayer& res );

private:
	uint8_t DelayTimer = 0;
	uint8_t SoundTimer = 0;

	uint16_t new_soundtimer_value = (uint16_t)-1;
};

#endif // TIMERS_H
