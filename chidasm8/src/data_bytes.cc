/*
 * data_bytes.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "data_bytes.h"


#include "utils.h"


void DataBytes::print( std::ostream &os ) const
{
	os << address << "\t\t"
	   << ".DB\t";

	for( uint8_t byte : byte_run ) os << format_byte( byte ) << " ";

	os << '\n';
}

std::ostream &operator<<( std::ostream &os, const DataBytes &data_bytes )
{
	data_bytes.print( os );

	return os;
}
