/*
 * target.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <string>
#include <ostream>

class Target
{
public:
	enum class eTargetKind { I_TARGET, SUBROUTINE, JUMP, INDEXED, UNKNOWN };

	/* Default arguments so we can make a Target object for comparison (find) */
	Target( uint16_t address, eTargetKind type = eTargetKind::UNKNOWN, std::string label = "" ) :
		address( address ), type( type ), label( label )
	{}

	bool operator<( const Target &rhs ) const { return address < rhs.address; }
	bool operator==( const Target &rhs ) const { return address == rhs.address; }

	std::string get_label() const { return label; }

	void print( std::ostream &os ) const;

private:
	uint16_t address;
	eTargetKind type;
	std::string label;
};

std::ostream& operator<<( std::ostream& os, const Target& target );

// enum class eTargetTypes { I_TARGET, SUBROUTINE, JUMP, INDEXED, UNKNOWN };
// using TargetMapKey = std::pair<eTargetTypes, uint16_t>;

// using TargetMap = std::unordered_map<