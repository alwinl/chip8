/*
 * asm_parser.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include "asm_ast.h"

#include "asm_token.h"

class ASMParser
{
public:
    ASMParser()  = default;

	ASTProgram parse_asm_tokens( const ASMTokens& tokens );

private:
	const ASMTokens * tokens = nullptr;
	ASMTokens::const_iterator cursor;

	ASTElement parse_line();
	ASTExpressions parse_expressions( int line );
	ASTExpression parse_expression();
	ASTExpression parse_additive();
	ASTExpression parse_multiplicative();
	ASTExpression parse_primary();

    ASMToken consume( ASMToken::Type type, const std::string& lexeme, const std::string& message );
	bool end_of_current_line( int start_line );
    bool match( ASMToken::Type type, std::string lexeme = "" );
    void forward_cursor();
    const ASMToken& peek() { return (cursor != tokens->end()) ? *cursor : tokens->back(); }
};
