/*
 * raw_data.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

class RawData
{
public:
	/* Default arguments so we can make a RawData object for comparison (find ) */
	RawData( uint16_t location, uint8_t value = 0, bool is_instruction = false )
	{
		this->location = location;
		this->value = value;
		this->is_instruction = is_instruction;
	};

	bool operator<( const RawData &rhs ) const { return location < rhs.location; }
	bool operator==( const RawData &rhs ) const { return location == rhs.location; }

	uint8_t val() const { return value; }
	uint16_t get_location() const { return location; }
	bool instruction() const { return is_instruction; }

private:
	uint16_t location;
	uint8_t value;
	bool is_instruction;
};
