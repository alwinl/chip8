/*
 * symbol_collector.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/symbol_collector.h"

#include "assembler/expression.h"

#include <cassert>

ASMSymbolTable collect_symbols( const ASTProgram& program )
{
	ASMSymbolTable symbols{};
	uint16_t address = 0x200;

	for( const ASTElement& element : program ) {

		if( element.label )
			symbols.define_label( element.label->name, address );

		address = std::visit( [&]( auto&& body ) -> uint16_t
		{
			using T = std::decay_t<decltype(body)>;

			if constexpr( std::is_same_v<T, ASTInstruction> ) {

				return address + 2;

			}
			else if constexpr( std::is_same_v<T, ASTDirective> ) {

				if( (body.name == ".DB") || (body.name == "DB") )
					address += body.args.size();

				if( (body.name == ".ORG") || (body.name == "ORG") )
					address = evaluate_expression( body.args[0], symbols );

				return address;

			}
			else if constexpr( std::is_same_v<T, ASTEqu> ) {

				symbols.define_constant( body.name, evaluate_expression( body.value, symbols ) );
				return address;

			}
			else if constexpr( std::is_same_v<T, ASTEmpty> ) {

				return address;

			}
			else
				static_assert(std::is_same_v<T, void>, "Unhandled AST type");

		}, element.body );
	}

	return symbols;
}
