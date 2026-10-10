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

#include "assembler/loader.h"
#include "assembler/tokeniser.h"
#include "assembler/parser.h"

#include "assembler/ast.h"
#include "assembler/symbol_table.h"

#include "assembler/expression.h"
#include "assembler/instruction_generator.h"
#include "assembler/symbol_collector.h"
#include "assembler/ir_generator.h"

#include "ir/chip8ir.h"

namespace assembler
{
	IRBundle build_ir( std::istream& is )
	{
		ASMSource source = load_assembly_source( is );
		ASMTokens tokens = tokenise_assembly( source );
		ASTProgram program = parse_tokens(tokens);

		ASMSymbolTable symbols = collect_symbols( program );
		IRProgram ir = generate_ir( program, symbols );

		return { std::move(ir), std::make_unique<ASMSymbolTable>(std::move(symbols)) };
	}
}
