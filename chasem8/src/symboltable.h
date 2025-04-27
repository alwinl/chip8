/*
 * symboltable.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <unordered_map>
#include <cstdint>
#include <istream>

class SymbolTable
{
public:
	SymbolTable() = default;

	void add_label( std::string label, uint16_t address );
	void add_variable( std::string name, uint16_t value );

	uint16_t get_address( std::string label ) const;
	uint8_t get_nibble( std::string name ) const;
	uint8_t get_byte( std::string name ) const;
	uint8_t get_word( std::string name ) const;

	size_t size() const { return symbol_table.size(); }

private:
	std::unordered_map<std::string, uint16_t> symbol_table;
};

