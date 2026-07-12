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

#include <unordered_map>
#include <cstdint>

#include "ir/chip8ir.h"
#include "ir/chip8formats.h"
#include "ir/ir_bundle.h"

#include "assembler/cmdlineparser.h"
#include "assembler/symbol_table.h"
#include "assembler/ast.h"

class Assembler
{
public:
	Assembler() = default;

	void configure( const ChasmCmdLineParser& cmd ) {};

	IRBundle build_ir( ASMSource source );

private:
	IRProgram ir;
	ASMSymbolTable symbols;

	void collect_symbols( const ASTProgram& program );
	uint16_t collect_symbols( const ASTInstruction& instruction, uint16_t address );
	uint16_t collect_symbols( const ASTDirective& directive, uint16_t address );
	uint16_t collect_symbols( const ASTEqu& equ, uint16_t address );
	uint16_t collect_symbols( const ASTEmpty&, uint16_t address );

	void generate_ir( const ASTProgram& program );
	uint16_t generate_ir( const ASTInstruction& instruction, uint16_t address );
	uint16_t generate_ir( const ASTDirective& directive, uint16_t address );
	uint16_t generate_ir( const ASTEqu&, uint16_t address );
	uint16_t generate_ir( const ASTEmpty&, uint16_t address );
};
