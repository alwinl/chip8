/*
 * symboltable.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "symboltable.h"

void SymbolTable::add_label( std::string label, uint16_t address )
{
    if( label.back() == ':' )
        label.pop_back();
        
	auto entry = symbol_table.find( label );
	if( entry != symbol_table.end() )
		throw std::invalid_argument("Duplicate label (" + label + ")");

	symbol_table[label] = address;
}

void SymbolTable::add_variable( std::string name, uint16_t value )
{
	auto entry = symbol_table.find( name );
	if( entry != symbol_table.end() )
		throw std::invalid_argument("Duplicate variable (" + name + ")");

	symbol_table[name] = value;
}

uint16_t SymbolTable::get_address( std::string label ) const
{
	auto entry = symbol_table.find( label );
	if( entry != symbol_table.end() )
		return ( *entry ).second;

	throw std::runtime_error("Non existent label (" + label + ")");
}

uint8_t SymbolTable::get_nibble( std::string name ) const
{
	auto entry = symbol_table.find( name );
	if( entry == symbol_table.end() )
		throw std::runtime_error("Non existent variable (" + name + ")");

	if( (*entry).second > 0x0F)
        throw std::runtime_error("Nibble value out of range (0-F)");

	return static_cast<uint8_t>((*entry).second);
}

uint8_t SymbolTable::get_byte( std::string name ) const
{
	auto entry = symbol_table.find( name );
	if( entry == symbol_table.end() )
		throw std::runtime_error("Non existent variable (" + name + ")");

	if( (*entry).second > 0xFF)
        throw std::runtime_error("Byte value out of range (0-FF)");

	return static_cast<uint8_t>( (*entry).second );
}

uint8_t SymbolTable::get_word( std::string name ) const
{
	auto entry = symbol_table.find( name );
	if( entry == symbol_table.end() )
		throw std::runtime_error("Non existent variable (" + name + ")");

	return (*entry).second;
}
