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

#include "assembler/expression.h"

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

	return -1;
}

std::string ASMSymbolTable::get_label( uint16_t address ) const
{
	if( auto it = address_label.find( address); it != address_label.end() )
		return it->second;

	return "";
}

uint16_t collect_symbols( ASMSymbolTable& symbols, const ASTInstruction& instruction, uint16_t address )
{
	return address + 2;
}

uint16_t collect_symbols( ASMSymbolTable& symbols, const ASTDirective& directive, uint16_t address )
{
	if( (directive.name == ".DB") || (directive.name == "DB") )
		address += directive.args.size();

	if( (directive.name == ".ORG") || (directive.name == "ORG") )
		address = evaluate_expression( directive.args[0], symbols );

	return address;
}

uint16_t collect_symbols( ASMSymbolTable& symbols, const ASTEqu& equ, uint16_t address )
{
	symbols.define_constant( equ.name, evaluate_expression( equ.value, symbols ) );

	return address;
}

uint16_t collect_symbols( ASMSymbolTable& symbols, const ASTEmpty&, uint16_t address )
{
	return address;
}

ASMSymbolTable collect_symbols( const ASTProgram& program )
{
	ASMSymbolTable symbols{};
	uint16_t address = 0x200;

	for( const ASTElement& element : program ) {

		if( element.label )
			symbols.define_label( element.label->name, address );

		address = std::visit( [&]( auto&& body ) -> uint16_t { return collect_symbols( symbols, body, address ); }, element.body );
	}

	return symbols;
}
