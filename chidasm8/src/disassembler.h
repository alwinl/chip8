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

#include <set>
#include <stack>
#include <string>
#include <vector>

#include <cstdint>

#include <iosfwd>

#include "instruction.h"
#include "data_bytes.h"
#include "target.h"
#include "memory.h"

struct InputData
{
	InputData( std::string bin_name, uint16_t origin ) : bin_name(bin_name), origin(origin ), memory(origin) {};

	std::string bin_name;
	uint16_t origin;
	Memory memory;
};

std::istream& operator>>( std::istream& is, InputData& input );

struct OutputData
{
	std::string bin_name;
	uint16_t origin;
	std::set<Instruction> instructions;
	std::set<DataBytes> databytes;
	std::set<Target> jmp_targets;
};

std::ostream& operator<<( std::ostream& os, const OutputData& data );
class Disassembler
{
public:
	Disassembler( InputData& input ) : memory(input.memory), bin_name(input.bin_name), origin(input.origin) {}

    OutputData disassemble();

private:
	uint16_t origin;
	std::string bin_name;
	Memory& memory;
	std::set<Instruction> instructions;
	std::set<DataBytes> databytes;

	std::stack<uint16_t> address_stack;
	// uint8_t V0_content; // need to keep a record of all registers, so we can push the correct address on JMP instruction

	/* Don't like this whole block
	 * maybe it should be encapsulated in a new object
	 */
	std::set<Target> jmp_targets;
	unsigned int label_sequence = 0;
	unsigned int funct_sequence = 0;
	unsigned int data_sequence = 0;
	unsigned int table_sequence = 0;
	std::string add_target( uint16_t target_address, Target::eTargetKind type );

	Instruction decode_SYS( uint16_t address, uint16_t opcode );
	Instruction decode_JP( uint16_t address, uint16_t opcode );
	Instruction decode_CALL( uint16_t address, uint16_t opcode );
	Instruction decode_SEI( uint16_t address, uint16_t opcode );
	Instruction decode_SNEI( uint16_t address, uint16_t opcode );
	Instruction decode_SER( uint16_t address, uint16_t opcode );
	Instruction decode_LD( uint16_t address, uint16_t opcode );
	Instruction decode_ADD( uint16_t address, uint16_t opcode );
	Instruction decode_MathOp( uint16_t address, uint16_t opcode );
	Instruction decode_SNE( uint16_t address, uint16_t opcode );
	Instruction decode_LDI( uint16_t address, uint16_t opcode );
	Instruction decode_JMP( uint16_t address, uint16_t opcode );
	Instruction decode_RND( uint16_t address, uint16_t opcode );
	Instruction decode_DRW( uint16_t address, uint16_t opcode );
	Instruction decode_Key( uint16_t address, uint16_t opcode );
	Instruction decode_Misc( uint16_t address, uint16_t opcode );

	void disassemble_instruction( uint16_t address );

	void collect_data_bytes();
};
