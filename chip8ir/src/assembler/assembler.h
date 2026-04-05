/*
 * assembler.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/chip8ir.h"
#include "ir/chip8formats.h"
#include "ir/ir_bundle.h"

#include "assembler/cmdlineparser.h"
#include "assembler/ast.h"


class Assembler
{
public:
	Assembler() = default;

	void configure( const ChasmCmdLineParser& cmd ) {};

	IRBundle build_ir( ASMSource source );

private:
	void process_pass1( IRBundle& bundle, const ASTProgram& program );
	void process_pass2( IRBundle& bundle, const ASTProgram& program );

	// uint16_t evaluate_expression( const IRBundle& bundle, const ASTExpression& expr );
	// Instruction build_instruction( const ASTInstruction& ast_inst, const IRBundle& bundle );

	// Reg parse_reg( const IRBundle &bundle, const ASTExpression& expr );
	// Addr parse_addr( const IRBundle &bundle, const ASTExpression& expr );
	// Imm parse_imm( const IRBundle &bundle, const ASTExpression& expr );
	// Nibble parse_nibble( const IRBundle &bundle, const ASTExpression& expr );
	// Key parse_key( const IRBundle &bundle, const ASTExpression& expr );
	// RegCount parse_regcount( const IRBundle &bundle, const ASTExpression& expr );

	// bool is_register( const ASTExpression& expr );
};
