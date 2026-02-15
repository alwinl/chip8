/*
 * assembler.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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


#include "assembler/assembler.h"

#include "assembler/tokeniser.h"
#include "assembler/parser.h"

AssemblyResult Assembler::build_ir( ASMSource source )
{
	IRProgram ir;
	SymbolTable symbols;

    ASMTokens tokens = ASMTokeniser().tokenise(source);
    ASTProgram program = ASMParser().parse_asm_tokens(tokens);

	process_pass1( ir, symbols, program );
	process_pass2( ir, symbols, program );

	return { ir, symbols };
}

void Assembler::process_pass1( IRProgram& ir, SymbolTable& symbols, const ASTProgram& program )
{
	uint16_t address = 0;

	for( const ASTElement& element : program ) {
		if( element.label ) {
			const std::string& name = element.label->name;
			// if( symbols.contains( name ) )

		}
		// element.body

	}
}

void Assembler::process_pass2( IRProgram& ir, SymbolTable& symbols, const ASTProgram& program )
{
	for( const ASTElement& element : program ) {

	}
}

        // std::visit( [&]( const auto& v ) { process_element( ir, symbols, line, label, body ); }, element );
