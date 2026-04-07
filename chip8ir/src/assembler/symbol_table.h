/*
 * asm_symbol_table.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include <unordered_map>

#include "ir/label_resolver.h"


class ASMSymbolTable : public ILabelResolver
{
public:
	ASMSymbolTable() = default;

	void define_label( std::string name, uint16_t address );
	void define_constant( std::string name, uint16_t value );

	uint16_t get_value( std::string name ) const;
	std::string get_label( uint16_t address ) const;

private:
	std::unordered_map<uint16_t, std::string> symbol_map;
	std::unordered_map<std::string, uint16_t> value_map;
};
