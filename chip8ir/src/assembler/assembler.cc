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
#include "assembler/symbol_table.h"
#include "assembler/expression.h"
#include "assembler/dispatcher.h"

uint16_t process_pass1( const ASTInstruction& instruction, uint16_t address, ASMSymbolTable& symbols )
{
	return address + 2;
}

uint16_t process_pass2( const ASTInstruction& instruction, uint16_t address, IRBundle& bundle )
{
	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);
	const auto& dispatcher = get_dispatcher();
	auto entry = dispatcher.find( instruction.mnemonic );

	if( entry == dispatcher.end() )
		throw std::runtime_error( "Unknown mnemonic: " + instruction.mnemonic );

	bundle.ir.elements.push_back( InstructionElement { address, entry->second( instruction, symbols ) } );

	return address + 2;
}

uint16_t process_pass1( const ASTDirective& directive, uint16_t address, ASMSymbolTable& symbols )
{
	if( directive.name == ".DB" )
		address += directive.args.size();

	if( directive.name == ".ORG" )
		address = evaluate_expression( directive.args[0], symbols );

	return address;
}

uint16_t process_pass2( const ASTDirective& directive, uint16_t address, IRBundle& bundle )
{
	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);

	if( directive.name == ".DB" ) {

		std::vector<uint8_t> byte_run;

		for( size_t i = 0; i < directive.args.size(); ++i )
			byte_run.emplace_back( evaluate_expression( directive.args[i], symbols ));

		bundle.ir.elements.push_back( DataElement {address, byte_run } );

		address += directive.args.size();
	}

	if( directive.name == ".ORG" )
		address = evaluate_expression( directive.args[0], symbols );

	return address;
}

uint16_t process_pass1( const ASTEqu& equ, uint16_t address, ASMSymbolTable& symbols )
{
	symbols.define_constant( equ.name, evaluate_expression( equ.value, symbols ) );

	return address;
}

uint16_t process_pass2( const ASTEqu&, uint16_t address, IRBundle& bundle )
{
	return address;
}

uint16_t process_pass1( const ASTEmpty&, uint16_t address, ASMSymbolTable& symbols )
{
	return address;
}

uint16_t process_pass2( const ASTEmpty&, uint16_t address, IRBundle& bundle )
{
	return address;
}

IRBundle Assembler::build_ir( ASMSource source )
{
    ASMTokens tokens = ASMTokeniser().tokenise(source);
    ASTProgram program = ASMParser().parse_asm_tokens(tokens);

	IRBundle bundle { {}, std::make_unique<ASMSymbolTable>() };

	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);

	// Pass 1
	uint16_t address = 0x200;

	for( const ASTElement& element : program ) {

		if( element.label )
			symbols.define_label( element.label->name, address );

		address = std::visit( [&]( auto&& body ) -> uint16_t { return process_pass1( body, address, symbols ); }, element.body );
	}

	// Pass 2
	address = 0x200;

	for( const ASTElement& element : program )
		address = std::visit( [&]( auto&& body ) -> uint16_t { return process_pass2( body, address, bundle ); }, element.body );

	return bundle;
}
