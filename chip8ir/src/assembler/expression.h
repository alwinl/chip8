/*
 * expression.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/ast.h"
#include "assembler/symbol_table.h"

#include "ir/chip8ir.h"

uint16_t evaluate_expression( const ASTNumberExpr& expr, const ASMSymbolTable& symbols );
uint16_t evaluate_expression( const ASTIdentifierExpr& expr, const ASMSymbolTable& symbols );
uint16_t evaluate_expression( const ASTExpression& expr, const ASMSymbolTable& symbols );
uint16_t evaluate_expression( const ASTBinaryExpr& expr, const ASMSymbolTable& symbols );

bool is_register( const ASTExpression& expr );
bool is_identifier( const ASTExpression& expr, std::string content );

Reg parse_reg( const ASTExpression& expr );
Addr parse_addr( const ASTExpression& expr, const ASMSymbolTable &symbols );
Imm parse_imm( const ASTExpression& expr, const ASMSymbolTable &symbols );
Nibble parse_nibble( const ASTExpression& expr, const ASMSymbolTable &symbols );
Key parse_key( const ASTExpression& expr, const ASMSymbolTable &symbols );
RegCount parse_regcount( const ASTExpression& expr, const ASMSymbolTable &symbols );

