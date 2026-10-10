/*
 * expression.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/expression.h"

namespace {

	template<typename T>
	uint16_t evaluate_expression( const T&, const ASMSymbolTable& )
	{
		static_assert(std::is_same_v<T, void>, "Unhandled expression type");
		return 0;
	}

	uint16_t evaluate_expression( const ASTNumberExpr& expr, const ASMSymbolTable& symbols )
	{
		return expr.value;
	}

	uint16_t evaluate_expression( const ASTIdentifierExpr& expr, const ASMSymbolTable& symbols )
	{
		return symbols.get_value( expr.text );
	}

	uint16_t evaluate_expression( const ASTBinaryExpr& expr, const ASMSymbolTable& symbols )
	{
		auto lhs = evaluate_expression( *expr.lhs, symbols );
		auto rhs = evaluate_expression( *expr.rhs, symbols );

		switch( expr.op ) {
		case '+': return lhs + rhs;
		case '-': return lhs - rhs;
		case '*': return lhs * rhs;
		case '/':
			if( rhs == 0 )
				throw( std::runtime_error("Division by zero") );
			return lhs / rhs;
		}

		return 0;
	}
}

uint16_t evaluate_expression( const ASTExpression& expr, const ASMSymbolTable& symbols )
{
	return std::visit( [&]( auto&& expression ) -> uint16_t { return evaluate_expression( expression, symbols ); }, expr.expression );
}
