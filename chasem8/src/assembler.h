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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>

#include "instruction.h"

struct program_parts {
	std::string label;
	std::string mnemonic;
	std::string arguments;
};


class Assembler
{
public:
	Assembler() = default;

	void remove_slash_r( std::string &input );
	void remove_comments( std::string &input );

	std::vector<std::string> split( std::string input );

	program_parts identify(std::vector<std::string> parts);

	void read_source( std::istream &source );

	void write_binary( std::ostream &target );
	void write_listing( std::ostream &target );

private:
	void add_instruction( Instruction &inst ) { instructions.push_back( inst ); }

	std::vector<Instruction> instructions;
	std::vector<uint8_t> codes;
};

#endif // PROGRAM_H