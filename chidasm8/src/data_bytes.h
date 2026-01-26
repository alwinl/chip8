/*
 * data_bytes.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <vector>
#include <ostream>

/*
 * Instead of collecting bytes and then creating a datarun object I should be
 * creating a datarun object and add bytes.
 * Once full we insert the object into the set
 */
class DataBytes
{
public:
	DataBytes( unsigned int address, std::vector<uint8_t> byte_run ) :
		address( address ),	byte_run( byte_run )
	{}

	bool operator<( const DataBytes &rhs ) const { return address < rhs.address; };
	bool operator==( const DataBytes &rhs ) const { return address == rhs.address; };

	uint16_t get_address() const { return address; }

	void print( std::ostream &os, bool is_clean ) const
	{
		if( !is_clean )
			os << address;

		os << ".DB\t";

		for( uint8_t byte : byte_run )
			os << format_byte( byte ) << " ";

		os << '\n';
	};

private:
	uint16_t address;
	std::vector<uint8_t> byte_run;
};
