/*
 * asm_emitter.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <ostream>
#include <string>

#include "ir/chip8ir.h"
#include "ir/chip8formats.h"
#include "ir/symbol_table.h"

class ASMEmitter
{
public:
	enum class OutputMode { Assembly, Listing };
	struct Config {
		std::string bin_name;
	};

	ASMEmitter() = default;

	void emit( std::ostream& os, const IRProgram& ir, const BinImage& bin_image, const SymbolTable& symbols, OutputMode mode );

	void configure( Config config ) { configuration = std::move(config); };

private:
	struct EmitContext {
		std::ostream& os;
		const IRProgram& ir;
		const BinImage& bin_image;
		const SymbolTable& symbols;
		OutputMode mode;
	};

	Config configuration;

	void emit_header( const EmitContext& ctx, std::string name );
	void emit_element( const EmitContext& ctx, const InstructionElement& element );
	void emit_element( const EmitContext& ctx, const DataElement& element );
	void emit_address( const EmitContext& ctx, uint16_t address );
	void emit_opcode( const EmitContext& ctx, uint16_t address );
	void emit_label( const EmitContext& ctx, uint16_t address );
	void emit_mnemonic( const EmitContext& ctx, const Opcode& opcode );
	void emit_operand( const EmitContext& ctx, const Operand& op );
};
