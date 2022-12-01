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

#include "timers.h"

#include "resourcelayer.h"

void Timers::set_delay_timer( uint8_t value )
{
	DelayTimer = value;
}

uint8_t Timers::get_delay_timer( ) const
{
	return DelayTimer;
}

void Timers::set_sound_timer( uint8_t value )
{
	SoundTimer = value;
	if( value )
		res.audio( true );
}

uint8_t Timers::get_sound_timer( ) const
{
	return SoundTimer;
}

void Timers::decrease_timers()
{
	if( DelayTimer )
		--DelayTimer;

	if( SoundTimer ) {
		--SoundTimer;
		if( !SoundTimer )
			res.audio( false );
	}
}
