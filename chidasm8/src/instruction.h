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

class Instruction
{
public:
	/* Default arguments so we can make an Instruction object for comparison (find) */
	Instruction( unsigned int address, unsigned int opcode = 0, std::string mnemonic = "", std::string argument = "" ) :
		address(address), opcode(opcode), mnemonic(mnemonic), argument(argument)
	{}

	bool operator<( const Instruction &rhs ) const { return address < rhs.address; };
	bool operator==( const Instruction &rhs ) const { return address == rhs.address; };

	uint16_t get_address() const { return address; };

	void print( std::ostream &os ) const;

private:
	uint16_t address;
	uint16_t opcode;
	std::string mnemonic;
	std::string argument;
};

std::ostream& operator<<( std::ostream& os, const Instruction& inst );

