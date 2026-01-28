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
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <variant>
#include <iosfwd>

#include "targets.h"
#include "memory.h"
#include "instruction.h"

struct DataBytes
{
	uint16_t address;
	std::vector<uint8_t> byte_run;
};

struct ASMElement
{
    uint16_t address;
    std::variant<Instruction, DataBytes> value;
};

class Disassembler
{
public:
	Disassembler( std::string bin_name, uint16_t origin ) :  bin_name(bin_name), origin(origin), memory( origin ) {}

	void read_input( std::istream& is );
	void disassemble();
	void print_output( std::ostream& os, bool is_clean );

private:
	uint16_t origin;
	std::string bin_name;
	Memory memory;
	Targets targets;
	std::vector<ASMElement> elements;

	void collect_instructions();
	void collect_data_bytes();
};
