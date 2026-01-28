/*
 * newtarget.h Copyright 2026 Alwin Leerling <dna.leerling@gmail.com>
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

#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <optional>

enum eKind { I_TARGET, SUBROUTINE, JUMP, INDEXED, COUNT };

struct DecodedTarget
{
	uint16_t address;
	eKind kind;
};

class Targets
{
public:
	Targets() = default;

	void add( std::optional<DecodedTarget> target );
	void sort_vectors();

	std::string get_label( uint16_t address ) const;

	std::vector<uint16_t> get_index_list( ) { return target_lists[INDEXED]; };

private:
	using ListArray = std::array<std::vector<uint16_t>, eKind::COUNT>;

	bool sorted = false;
	ListArray target_lists;
};
