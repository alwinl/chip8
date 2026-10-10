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
#include <cassert>

uint16_t evaluate_expression( const ASTExpression& expr, const ASMSymbolTable& symbols )
{
	return std::visit( [&]( auto&& expression ) -> uint16_t
		{
			using T = std::decay_t<decltype(expression)>;

			if constexpr( std::is_same_v<T, ASTNumberExpr> )
				return expression.value;

			else if constexpr( std::is_same_v<T, ASTIdentifierExpr> )
				return symbols.get_value( expression.text );

			else if constexpr( std::is_same_v<T, ASTBinaryExpr> ) {

				auto lhs = evaluate_expression( *expression.lhs, symbols );
				auto rhs = evaluate_expression( *expression.rhs, symbols );

				switch( expression.op ) {
				case '+': return lhs + rhs;
				case '-': return lhs - rhs;
				case '*': return lhs * rhs;
				case '/':
					if( rhs == 0 )
						throw( std::runtime_error("Division by zero") );
					return lhs / rhs;
				}

				throw( std::runtime_error("Invalid operator") );

			} else
				static_assert(std::is_same_v<T, void>, "Unhandled expression type");

		}, expr.expression );
}
