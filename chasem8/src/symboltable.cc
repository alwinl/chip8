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
        
    symbol_table[label] = address;
}

uint16_t SymbolTable::get_address( std::string label ) const
{
	auto entry = symbol_table.find( label );
	if( entry != symbol_table.end() )
		return ( *entry ).second;

	return (uint16_t)-1;
}

