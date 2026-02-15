/*
 * disassembler.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <cassert>

#include "ir/chip8ir.h"
#include "ir/chip8formats.h"
#include "ir/symbol_table.h"

#include "disassembler/memory.h"

struct DisassemblyResult
{
	IRProgram ir;
	SymbolTable symbols;
};

class Disassembler
{
public:
	struct Config {
		uint16_t origin;
	};

	Disassembler() = default;

	void configure( Config config ) { configuration = std::move(config); };

	DisassemblyResult build_ir( const BinImage& binary );

private:
	Config configuration;

	void collect_instructions( IRProgram& ir, DisasmMemory& memory, SymbolTable& symbols );
	void collect_data_bytes( IRProgram& ir, DisasmMemory& memory, SymbolTable& symbols );
	void sort_elements( IRProgram& ir );
};