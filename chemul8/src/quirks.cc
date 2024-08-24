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

#include "quirks.h"

Quirks::Quirks ( Quirks::eChipType type )
{
	switch (type)
	{
	case eChipType::CHIP8: quirks = CHIP8_quirks; break;
	case eChipType::XOCHIP: quirks = XOCHIP8_quirks; break;
	case eChipType::SCHIP: quirks = SCHIP8_quirks; break;
	}
}

bool Quirks::has_quirk( Quirks::eQuirks test_quirk )
{
	switch( test_quirk ) {
	case eQuirks::RESET: return quirks.reset_quirk;
	case eQuirks::MEMORY: return quirks.memory_quirk;
	case eQuirks::DISP_WAIT: return quirks.display_wait_quirk;
	case eQuirks::CLIPPING: return quirks.clipping_quirk;
	case eQuirks::SHIFTING: return quirks.shifting_quirk;
	case eQuirks::JUMPING: return quirks.jumping_quirk;
	};

	return false;
}

