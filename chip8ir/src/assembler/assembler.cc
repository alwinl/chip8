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


IRBundle Assembler::build_ir( ASMSource source )
{
    ASMTokens tokens = ASMTokeniser().tokenise(source);
    ASTProgram program = ASMParser().parse_asm_tokens(tokens);

	IRBundle bundle { {}, std::make_unique<ASMSymbolTable>() };

	process_pass1( bundle, program );
	process_pass2( bundle, program );

	return bundle;
}

void Assembler::process_pass1( IRBundle& bundle, const ASTProgram& program )
{
	ASMSymbolTable* symbols = dynamic_cast<ASMSymbolTable*>( bundle.resolver.get() );
	uint16_t address = 0;

	for( const ASTElement& element : program )
	{
		if( element.label )
			symbols->define_label( element.label->name, address );

		std::visit( [&]( auto&& body )
		{
			using T = std::decay_t<decltype(body)>;

			if constexpr ( std::is_same_v<T, ASTInstruction> )
			{
				address += 2;
			}
			else if constexpr ( std::is_same_v<T, ASTDirective> )
			{
				if( body.name == ".DB" )
					address += body.args.size();
				if( body.name == ".ORG" )
					address = evaluate_expression( bundle, body.args[0] );
			}
			else if constexpr ( std::is_same_v<T, ASTEqu> )
			{
				symbols->define_constant( body.name, evaluate_expression( bundle, body.value ) );
			}

		}, element.body );
	}
}

void Assembler::process_pass2( IRBundle& bundle, const ASTProgram& program )
{
	for( const ASTElement& element : program ) {

		std::visit( [&]( auto&& body )
		{
			using T = std::decay_t<decltype(body)>;

			if constexpr ( std::is_same_v<T, ASTInstruction> )
			{
			}
			else if constexpr ( std::is_same_v<T, ASTDirective> )
			{
			}
			else if constexpr ( std::is_same_v<T, ASTEqu> )
			{
			}

		}, element.body );

	}
}

uint16_t Assembler::evaluate_expression( IRBundle& bundle, const ASTExpression& expr )
{
	ASMSymbolTable* symbols = dynamic_cast<ASMSymbolTable*>( bundle.resolver.get() );

	return std::visit( [&]( auto&& expression ) -> uint16_t
	{
		using T = std::decay_t<decltype(expression)>;

		if constexpr ( std::is_same_v<T, NumberExpr> )
		{
			return expression.value;
		}
		else if constexpr ( std::is_same_v<T, IdentifierExpr> )
		{
			return symbols->get_value( expression.text );
		}
		else if constexpr ( std::is_same_v<T, ASTBinaryExpr> )
		{
			auto lhs = evaluate_expression( bundle, *expression.lhs.get() );
			auto rhs = evaluate_expression( bundle, *expression.rhs.get() );

			switch( expression.op ) {
			case '+': return lhs + rhs;
			case '-': return lhs - rhs;
			case '*': return lhs * rhs;
			case '/': return rhs!= 0 ? lhs / rhs : 0;
			}

			return 0;
		}
		return 0;

	}, expr.expression );
}
