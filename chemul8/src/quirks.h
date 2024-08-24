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

#ifndef QUIRKS_H
#define QUIRKS_H

class Quirks {
 public:
	enum class eChipType { CHIP8, XOCHIP, SCHIP };
	Quirks( Quirks::eChipType type);

	enum class eQuirks { RESET, MEMORY, DISP_WAIT, CLIPPING, SHIFTING, JUMPING };
	bool has_quirk( eQuirks test_quirk );

 private:
	struct QuirkFlags {
		bool reset_quirk;
		bool memory_quirk;
		bool display_wait_quirk;
		bool clipping_quirk;
		bool shifting_quirk;
		bool jumping_quirk;
	};

	static constexpr QuirkFlags CHIP8_quirks = {
		.reset_quirk = true,
		.memory_quirk = true,
		.display_wait_quirk = true,
		.clipping_quirk = true,
		.shifting_quirk = false,
		.jumping_quirk = false,
	};
	static constexpr QuirkFlags XOCHIP8_quirks = {
		.reset_quirk = false,
		.memory_quirk = true,
		.display_wait_quirk = false,
		.clipping_quirk = false,
		.shifting_quirk = false,
		.jumping_quirk = false,

	};
	static constexpr QuirkFlags SCHIP8_quirks = {
		.reset_quirk = false,
		.memory_quirk = false,
		.display_wait_quirk = false,
		.clipping_quirk = true,
		.shifting_quirk = true,
		.jumping_quirk = true,

	};

	QuirkFlags quirks;
};

#endif  // QUIRKS_H
