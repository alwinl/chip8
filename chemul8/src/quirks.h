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

#include <map>

class Quirks {
 public:
	enum class eChipType { CHIP8, XOCHIP, SCHIP };
	Quirks( Quirks::eChipType type) : type( type ) {};

	enum class eQuirks { RESET, MEMORY, DISP_WAIT, CLIPPING, SHIFTING, JUMPING };
	bool has_quirk( eQuirks test_quirk )
	{
		try {
			return quirk_map.at(type).at(test_quirk);
		}
		catch(...) {
			return false;
		}
	}

 private:
	Quirks::eChipType type;

	using QuirkMap = std::map<eQuirks,bool>;

	std::map<eChipType, QuirkMap> quirk_map = {
		{ eChipType::CHIP8, {
				{ eQuirks::RESET, true },
				{ eQuirks::MEMORY, true },
				{ eQuirks::DISP_WAIT, true },
				{ eQuirks::CLIPPING, true },
				{ eQuirks::SHIFTING, false },
				{ eQuirks::JUMPING, false }
			}
		},
		{ eChipType::XOCHIP, {
				{ eQuirks::RESET, false },
				{ eQuirks::MEMORY, true },
				{ eQuirks::DISP_WAIT, false },
				{ eQuirks::CLIPPING, false },
				{ eQuirks::SHIFTING, false },
				{ eQuirks::JUMPING, false }
			}
		},
		{ eChipType::SCHIP, {
				{ eQuirks::RESET, false },
				{ eQuirks::MEMORY, false },
				{ eQuirks::DISP_WAIT, false },
				{ eQuirks::CLIPPING, true },
				{ eQuirks::SHIFTING, true },
				{ eQuirks::JUMPING, true }
			}
		}
	};
};
