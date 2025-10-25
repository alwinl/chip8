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

#include "raw_data.h"
#include "instruction.h"
#include "data_bytes.h"
#include "target.h"

class Disassembler
{
public:
	Disassembler( std::string bin_name, unsigned int origin ) :	bin_name(bin_name), origin(origin) {};

	void read_binary( std::istream &is );
	void disassemble();
	void collect_data_bytes();
	void write_listing( std::ostream &os ) const;

private:
	std::string bin_name;
	uint16_t origin;

	std::set<RawData> data_set;
	std::set<Instruction> instructions;
	std::set<DataBytes> databytes;

	std::stack<uint16_t> address_stack;
	uint8_t V0_content; // need to keep a record of all registers, so we can push the correct address on JMP instruction

	/* Don't like this whole block
	 * maybe it should be encapsulated in a new object
	 */
	std::set<Target> jmp_targets;
	unsigned int label_sequence = 0;
	unsigned int funct_sequence = 0;
	unsigned int data_sequence = 0;
	unsigned int table_sequence = 0;
	std::string add_target( uint16_t source_address, uint16_t target_address, Target::eTargetKind type );

	/*
	 * Problem with all these decode functions is that they have two side effects
	 * 1. Translate a raw byte into a mnemonic
	 * 2. Calculate new instruction addresses
	 *
	 * These are to distinct responsibilities
	 */
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

	void mark_as_instruction( uint16_t address );
	void disassemble_instruction( uint16_t address );

	void configure_stream( std::ostream &os ) const;
	void write_header( std::ostream &os ) const;

	void output_instruction( std::ostream &os, Instruction inst ) const;
	void output_datarun( std::ostream &os, DataBytes datarun ) const;

	bool label_present( std::set<Target>::iterator &it, uint16_t address ) const;
};
