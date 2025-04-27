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

#pragma once

#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include <memory>

#include <unordered_map>

#include "instruction.h"

#include "symboltable.h"

class Assembler
{
public:
	Assembler() = default;

	void read_source( std::istream &source );
	void write_binary( std::ostream &target );
	void write_listing( std::ostream &target );

protected:
	void extract_label( std::vector<std::string> &tokens, uint16_t current_address );
	void extract_variable( std::vector<std::string> &tokens );
	uint16_t extract_instruction( std::vector<std::string> &tokens );

	uint16_t get_instruction_count() const { return instructions.size(); }
	uint16_t get_symbol( const std::string &label ) const { return symbol_table.get_address( label ); }

private:
	std::vector<std::unique_ptr<Instruction>> instructions;
	SymbolTable symbol_table;
};
