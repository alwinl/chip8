/*
 * symbol_table.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/symbol_table.h"

#include <stdexcept>

void ASMSymbolTable::define_label( std::string name, uint16_t address )
{
	if( constants.find( name) != constants.end() || label_address.find( name) != label_address.end() )
		return;

	address_label.insert( std::make_pair( address, name ));
	label_address.insert( std::make_pair( name, address ));
}

void ASMSymbolTable::define_constant( std::string name, uint16_t value )
{
	if( constants.find( name) != constants.end() || label_address.find( name) != label_address.end() )
		return;

	constants.insert( std::make_pair( name, value ));
}

uint16_t ASMSymbolTable::get_value( const std::string& name ) const
{
	if( auto it = constants.find( name); it != constants.end() )
		return it->second;

	if( auto it = label_address.find( name); it != label_address.end() )
		return it->second;

	throw std::runtime_error("Undefined symbol: " + name);
}

std::string ASMSymbolTable::get_label( uint16_t address ) const
{
	if( auto it = address_label.find( address); it != address_label.end() )
		return it->second;

	return "";
}
