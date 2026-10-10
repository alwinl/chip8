/*
 * ir_generator.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/ir_generator.h"

#include <cstdint>

#include "instruction_generator.h"
#include "expression.h"

IRProgram generate_ir( const ASTProgram& program, const ASMSymbolTable& symbols )
{
	IRProgram ir{};
	uint16_t address = 0x200;

	for( const ASTElement& element : program )
		address = std::visit( [&]( auto&& body ) -> uint16_t
		{
			using T = std::decay_t<decltype(body)>;

			if constexpr( std::is_same_v<T, ASTInstruction> ) {

				Instruction generated = generate_instruction(body, symbols);
				ir.elements.push_back( IRInstruction{address, std::move(generated)});
				return address + 2;

			}
			else if constexpr( std::is_same_v<T, ASTDirective> ) {

				if( (body.name == ".DB") || (body.name == "DB") ) {

					std::vector<uint8_t> byte_run;

					for( size_t i = 0; i < body.args.size(); ++i )
						byte_run.emplace_back( evaluate_expression( body.args[i], symbols ));

					ir.elements.push_back( IRDataBlock {address, byte_run } );

					address += body.args.size();
				}

				if( (body.name == ".ORG") || (body.name == "ORG") )
					address = evaluate_expression( body.args[0], symbols );

				return address;

			}
			else if constexpr( std::is_same_v<T, ASTEqu> ) {

				return address;

			}
			else if constexpr( std::is_same_v<T, ASTEmpty> ) {

				return address;

			}
			else
				static_assert(std::is_same_v<T, void>, "Unhandled AST type");

		}, element.body );

	return ir;
}
