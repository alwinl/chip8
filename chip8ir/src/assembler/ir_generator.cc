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

namespace {

	uint16_t generate_ir( IRProgram& ir, const ASTInstruction& instruction, uint16_t address, const ASMSymbolTable& symbols )
	{
		Instruction generated = generate_instruction(instruction, symbols);

		ir.elements.push_back( InstructionElement{address, std::move(generated)});

		return address + 2;
	}

	uint16_t generate_ir( IRProgram& ir, const ASTDirective& directive, uint16_t address, const ASMSymbolTable& symbols )
	{
		if( (directive.name == ".DB") || (directive.name == "DB") ) {

			std::vector<uint8_t> byte_run;

			for( size_t i = 0; i < directive.args.size(); ++i )
				byte_run.emplace_back( evaluate_expression( directive.args[i], symbols ));

			ir.elements.push_back( DataElement {address, byte_run } );

			address += directive.args.size();
		}

		if( (directive.name == ".ORG") || (directive.name == "ORG") )
			address = evaluate_expression( directive.args[0], symbols );

		return address;
	}

	uint16_t generate_ir( IRProgram& ir, const ASTEqu&, uint16_t address, const ASMSymbolTable& symbols )
	{
		return address;
	}

	uint16_t generate_ir( IRProgram& ir, const ASTEmpty&, uint16_t address, const ASMSymbolTable& symbols )
	{
		return address;
	}
}

IRProgram generate_ir( const ASTProgram& program, const ASMSymbolTable& symbols )
{
	IRProgram ir{};
	uint16_t address = 0x200;

	for( const ASTElement& element : program )
		address = std::visit( [&]( auto&& body ) -> uint16_t { return generate_ir( ir, body, address, symbols ); }, element.body );

	return ir;
}
