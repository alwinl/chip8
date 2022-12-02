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

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <vector>
#include <set>
#include <stack>

#include <iosfwd>

class RawData
{
public:
    /* Default arguments so we can make a RawData object for comparison (find ) */
	RawData( uint16_t location, uint8_t value = 0, bool is_instruction  = false )
	{
		this->location = location;
		this->value = value;
		this->is_instruction = is_instruction;
	};

	bool operator<( const RawData& rhs ) const { return location < rhs.location; }
	bool operator==( const RawData& rhs ) const { return location == rhs.location; }

	uint8_t val() const { return value; }
	uint16_t get_location() const { return location; }
	bool instruction() const { return is_instruction; }

private:
	uint16_t location;
	uint8_t value;
	bool is_instruction;
};

class Instruction
{
public:
    /* Default arguments so we can make an Instruction object for comparison (find) */
	Instruction( unsigned int address, std::string mnemonic = "", std::string argument = "" )
	{
		this->address = address;
		this->mnemonic = mnemonic;
		this->argument = argument;
	}

	uint16_t get_address() const { return address; };
	std::string get_mnemonic() const { return mnemonic; };
	std::string get_argument() const { return argument; };

	bool operator<( const Instruction& rhs ) const { return address < rhs.address; };
	bool operator==( const Instruction& rhs ) const { return address == rhs.address; };

	void print( std::ostream& os );

private:
	uint16_t address;
	std::string mnemonic;
	std::string argument;
};

/*
 * Instead of collecting bytes and then creating a datarun object I should be
 * creating a datarun object and add bytes.
 * Once full we insert the object into the set
 */
class DataBytes
{
public:
	DataBytes( unsigned int address, std::vector<uint8_t> byte_run )
	{
		this->address = address;
		this->byte_run = byte_run;
	}

	uint16_t get_address() const { return address; }
	std::vector<uint8_t> get_byte_run( ) const { return byte_run; }

	bool operator<( const DataBytes& rhs ) const { return address < rhs.address; };
	bool operator==( const DataBytes& rhs ) const { return address == rhs.address; };

	void print( std::ostream& os );

private:
	uint16_t address;
	std::vector<uint8_t> byte_run;
};

class Target
{
public:
    enum class eTargetKind { I_TARGET, SUBROUTINE, JUMP, INDEXED, UNKNOWN };

    /* Default arguments so we can make a Target object for comparison (find) */
    Target( uint16_t address, eTargetKind type = eTargetKind::UNKNOWN, std::string label = "" )
    {
        this->address = address;
        this->type = type;
        this->label = label;
    };

    bool operator<( const Target& rhs ) const { return address < rhs.address; }
    bool operator==( const Target& rhs ) const { return address == rhs.address; }

    uint16_t get_address() const { return address; }
    eTargetKind get_kind() const { return type; }
    std::string get_label() const { return label; }

    void print( std::ostream& os );

private:
    uint16_t address;
    eTargetKind type;
    std::string label;
};

class Disassembler
{
public:
	Disassembler( std::string bin_name, unsigned int origin )
	{
		this->bin_name = bin_name;
		this->origin = origin;
	};

	void read_binary( std::istream& is );
	void disassemble( );
	void write_listing( std::ostream& os ) const;

private:

	uint16_t origin;
	std::string bin_name;

	std::set<RawData> data_set;
	std::set<Instruction> instructions;
	std::set<DataBytes> databytes;

	std::stack<uint16_t> address_stack;

	/* Don't like this whole block
	 * maybe it should be encapsulated in a new object
	 */
	std::set<Target> jmp_targets;
	unsigned int label_sequence = 0;
	unsigned int funct_sequence = 0;
	unsigned int data_sequence = 0;
	unsigned int table_sequence = 0;
	std::string add_target( uint16_t source_address, uint16_t target_address, Target::eTargetKind type );


	void add_raw_byte( uint16_t address, uint8_t value );

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
	Instruction decode_SNI( uint16_t address, uint16_t opcode );
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

    void configure_stream( std::ostream& os ) const;
    void write_header( std::ostream& os ) const;

	void output_instruction( std::ostream& os, Instruction inst ) const;
	void output_datarun( std::ostream& os, DataBytes datarun ) const;

	bool label_present( std::set<Target>::iterator& it, uint16_t address ) const;
	void write_label( std::ostream& os, Target target ) const;
	void write_instruction( std::ostream& os, Instruction inst ) const;
	void write_datarun( std::ostream& os, DataBytes datarun ) const;
};

#endif // DISASSEMBLER_H
