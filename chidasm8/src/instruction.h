/*
 * instruction.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "utils.h"

class Instruction
{
public:
	/* Default arguments so we can make an Instruction object for comparison (find) */
	Instruction( unsigned int address, unsigned int opcode = 0, std::string mnemonic = "", std::string argument = "", uint16_t target_address = 0 ) :
		address(address), opcode(opcode), mnemonic(mnemonic), argument(argument), target_address(target_address)
	{}

	bool operator<( const Instruction &rhs ) const { return address < rhs.address; };
	bool operator==( const Instruction &rhs ) const { return address == rhs.address; };

	uint16_t get_address() const { return address; };
	uint16_t get_target_address() const { return target_address; }

	void print( std::ostream &os, std::string label ) const
	{
		uint8_t low_byte = opcode & 0xFF;
		uint8_t high_byte = (opcode >> 8) & 0xFF;

		os << address << "\t\t" << format_naked_byte( high_byte ) << " " << format_naked_byte( low_byte ) << "\t"
			<< mnemonic << " " << argument << label << "\n";
	};

private:
	uint16_t address;
	uint16_t opcode;
	std::string mnemonic;
	std::string argument;
	uint16_t target_address;
};
