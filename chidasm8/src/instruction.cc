/*
 * instruction.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "instruction.h"

#include <ostream>

#include "utils.h"

void Instruction::print( std::ostream &os, Targets& targets ) const
{
	uint8_t low_byte = opcode & 0xFF;
	uint8_t high_byte = (opcode >> 8) & 0xFF;

	os << address << "\t\t" << format_naked_byte( high_byte ) << " " << format_naked_byte( low_byte ) << "\t"
		<< mnemonic << " " << argument ;

	if( target_address )
		os << targets.get_label( target_address );

	os << "\n";
}

// std::ostream& operator<<( std::ostream& os, const Instruction& inst )
// {
// 	inst.print( os );

// 	return os;
// }
