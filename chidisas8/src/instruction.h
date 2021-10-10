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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>
//#include <iosfwd>

#if 0
class RawData
{
public:
	RawData( uint16_t location, uint8_t value, bool is_instruction )
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
	Instruction( unsigned int address, std::string mnemonic, std::string argument )
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

private:
	uint16_t address;
	std::string mnemonic;
	std::string argument;
};

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

private:
	uint16_t address;
	std::vector<uint8_t> byte_run;
};

#endif // 0
#endif // INSTRUCTION_H
