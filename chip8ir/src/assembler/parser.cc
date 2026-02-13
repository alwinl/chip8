/*
 * asm_parser.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <algorithm>

#include "assembler/parser.h"

ASTProgram ASMParser::parse_asm_tokens( const ASMTokens& tokens )
{
	ASTProgram program {};

	this->tokens = &tokens;
    cursor = tokens.begin();

    while( match( ASMToken::Type::COMMENT ) )
        ++cursor;

    while( ! match( ASMToken::Type::END_OF_INPUT ) )
        program.push_back( parse_line() );

    return program;
}

ASTElement ASMParser::parse_line()
{
	auto make_upper = [](unsigned char c){ return std::toupper(c); };

    ASTElement element {};

    element.line = peek().line;

	if( match( ASMToken::Type::LABEL) )
	{
		auto token = consume( ASMToken::Type::LABEL, "", "Expected label" );

		auto& label = element.label.emplace();
		label.name = token.lexeme;
		label.column = token.column;
	}

    if( !end_of_current_line(element.line) )
	{
		if( match( ASMToken::Type::DIRECTIVE) ) {

			auto token = consume( ASMToken::Type::DIRECTIVE, "", "Expected a directive" );

			std::transform( token.lexeme.begin(), token.lexeme.end(), token.lexeme.begin(), make_upper );

			element.body = ASTDirective { token.lexeme, token.column, std::move(parse_expressions(element.line)) };

        } else if( match(ASMToken::Type::IDENTIFIER) ) {

			auto token = consume( ASMToken::Type::IDENTIFIER, "", "Expected symbol name" );

			if( match( ASMToken::Type::ASSIGNMENT ) ) {
				consume( ASMToken::Type::ASSIGNMENT, "", "Expected assignment");
				std::transform( token.lexeme.begin(), token.lexeme.end(), token.lexeme.begin(), make_upper );
				element.body = ASTEqu { token.lexeme, token.column, std::move(parse_expression()) };
			} else {
				std::transform( token.lexeme.begin(), token.lexeme.end(), token.lexeme.begin(), make_upper );
				element.body = ASTInstruction { token.lexeme, token.column, std::move(parse_expressions(element.line)) };
			}

		} else {
			throw std::runtime_error(
				"Line " + std::to_string(cursor->line) + ":" + std::to_string(cursor->column) + " - syntax error"
			);
		}
    }

    while( !end_of_current_line(element.line) )
        forward_cursor();

    return element;
}

ASTExpressions ASMParser::parse_expressions(int line)
{
	ASTExpressions args;

	while( !end_of_current_line( line ) ) {

		if( !match(ASMToken::Type::IDENTIFIER) && !match(ASMToken::Type::NUMBER) )
			break;

		args.push_back( parse_expression() );

		if( match( ASMToken::Type::COMMA ) )
			consume( ASMToken::Type::COMMA, ",", "Expected comma" );
		else
			break;
	}

	return args;
}

ASTExpression ASMParser::parse_expression()
{
	return parse_additive();
}

ASTExpression ASMParser::parse_additive()
{
	ASTExpression lhs = parse_multiplicative();

	while( match( ASMToken::Type::OPERATOR, "+" ) || match( ASMToken::Type::OPERATOR, "-" ) ) {

		auto token = consume( ASMToken::Type::OPERATOR, "", "Expected operator" );

		ASTExpression rhs = parse_multiplicative();

		lhs = ASTExpression {
			ASTBinaryExpr {
				token.lexeme[0],
				std::make_unique<ASTExpression>( std::move(lhs) ),
				std::make_unique<ASTExpression>( std::move(rhs) )
			},
			token.column
		};
	}

	return lhs;
}

ASTExpression ASMParser::parse_multiplicative()
{
	ASTExpression lhs = parse_primary();

	while( match( ASMToken::Type::OPERATOR, "*" ) || match( ASMToken::Type::OPERATOR, "/" ) ) {

		auto token = consume( ASMToken::Type::OPERATOR, "", "Expected operator" );

		ASTExpression rhs = parse_primary();

		lhs = ASTExpression {
			ASTBinaryExpr {
				token.lexeme[0],
				std::make_unique<ASTExpression>( std::move(lhs) ),
				std::make_unique<ASTExpression>( std::move(rhs) )
			},
			token.column
		};
	}

	return lhs;
}

ASTExpression ASMParser::parse_primary()
{
	if( match(ASMToken::Type::NUMBER) )
	{

		int col = cursor->column;
        std::string lexeme = consume( ASMToken::Type::NUMBER, "", "Expected number" ).lexeme;

 		unsigned long tmp = 0;
		try {
			tmp = std::stoul( lexeme, nullptr, 0 ); // base 0 allows 0x for hex
		}
		catch( const std::exception& e ) {
			throw std::runtime_error(
				"Line " + std::to_string(cursor->line) + ":" + std::to_string(col) + " - Invalid number: " + lexeme
			);
		}

		if( tmp > UINT32_MAX ) {
			throw std::runtime_error(
				"Line " + std::to_string(cursor->line) + ":" + std::to_string(col) + " - Number out of range for uint32_t: " + lexeme
			);
		}

        return ASTExpression{ NumberExpr{ static_cast<uint32_t>(tmp) }, col };
	}

	if( match(ASMToken::Type::IDENTIFIER) )
	{
        int col = cursor->column;
        std::string lexeme = consume( ASMToken::Type::IDENTIFIER, "", "Expected identifier" ).lexeme;
        return ASTExpression{ IdentifierExpr{ lexeme }, col };
    }

    throw std::runtime_error(
        "Parse error at line " + std::to_string(peek().line) + ", column " + std::to_string(peek().column) + ": expected number or identifier"
    );
}

ASMToken ASMParser::consume(ASMToken::Type type, const std::string& lexeme, const std::string& message)
{
    if( match( type, lexeme ) ) {
        ASMToken token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error(
        "FileName:" + std::to_string(peek().line) + ":" + std::to_string(peek().column)
        + ": Parse error: " + message + " (expected '" + lexeme + "', got '" + peek().lexeme + "')" );
}

bool ASMParser::end_of_current_line(int start_line)
{
	return match(ASMToken::Type::END_OF_INPUT) || peek().line != start_line;
}

bool ASMParser::match( ASMToken::Type type, std::string lexeme )
{
    if( peek().type != type )
        return false;

    if( !lexeme.empty() && peek().lexeme != lexeme )
        return false;

    return true;
}

void ASMParser::forward_cursor()
{
    ++cursor;

    while( peek().type == ASMToken::Type::COMMENT )
        ++cursor;
}
