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

uint16_t evaluate_expression( const ASTExpression& expr, const ASMSymbolTable& symbols )
{
	return std::visit( [&]( auto&& expression ) -> uint16_t { return evaluate_expression( expression, symbols ); }, expr.expression );
}

uint16_t evaluate_expression( const ASTBinaryExpr& expr, const ASMSymbolTable& symbols )
{
	auto lhs = evaluate_expression( *expr.lhs, symbols );
	auto rhs = evaluate_expression( *expr.rhs, symbols );

	switch( expr.op ) {
	case '+': return lhs + rhs;
	case '-': return lhs - rhs;
	case '*': return lhs * rhs;
	case '/': return rhs!= 0 ? lhs / rhs : 0;
	}

	return 0;
}


bool is_register( const ASTExpression& expr )
{
    if( !std::holds_alternative<ASTIdentifierExpr>(expr.expression) )
        return false;

    const auto& id = std::get<ASTIdentifierExpr>(expr.expression).text;
    return !id.empty() && id[0] == 'V';
}

bool is_identifier( const ASTExpression& expr, std::string content )
{
    if( !std::holds_alternative<ASTIdentifierExpr>(expr.expression) )
        return false;

    const auto& id = std::get<ASTIdentifierExpr>(expr.expression).text;
    return !id.empty() && content == id;
}


Reg parse_reg( const ASTExpression& expr )
{
    if (!std::holds_alternative<ASTIdentifierExpr>(expr.expression))
        throw std::runtime_error("Expected register");

    const auto& id = std::get<ASTIdentifierExpr>(expr.expression).text;

    if (id.size() < 2 || id[0] != 'V')
        throw std::runtime_error("Invalid register: " + id);

    int index = std::stoi(id.substr(1));

    if (index < 0 || index > 15)
        throw std::runtime_error("Register out of range");

    return Reg{ static_cast<uint8_t>(index) };
}

Addr parse_addr( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
	return Addr{ evaluate_expression( expr, symbols ) };
}

Imm parse_imm( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    return Imm{ static_cast<uint8_t>( evaluate_expression( expr, symbols) ) };
}

Nibble parse_nibble( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    auto val = evaluate_expression( expr, symbols );
    if (val > 0xF)
		throw std::runtime_error("Nibble out of range");

    return Nibble{ static_cast<uint8_t>(val) };
}

Key parse_key( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    auto val = evaluate_expression(expr, symbols);
    if (val > 0xF)
		throw std::runtime_error("Key out of range");

    return Key{ static_cast<uint8_t>(val) };
}

RegCount parse_regcount( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    auto val = evaluate_expression( expr, symbols );
    if (val > 0xF)
		throw std::runtime_error("RegCount out of range");

    return RegCount{ static_cast<uint8_t>(val) };
}
