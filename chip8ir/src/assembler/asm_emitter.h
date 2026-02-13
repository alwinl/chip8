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

#include "chip8ir.h"
#include "chip8formats.h"

class ASMEmitter
{
public:
	enum class OutputMode { Assembly, Listing };
	struct Config {
		std::string bin_name;
	};

	ASMEmitter() = default;

	void emit( std::ostream& os, const IRProgram& ir, const BinImage& bin_image, OutputMode mode );

	void configure( Config config ) { configuration = std::move(config); };

private:
	Config configuration;

	void emit_label( std::ostream& os, const IRProgram& ir, uint16_t address );
	void emit_address( std::ostream& os, uint16_t address );
	void emit_opcode( std::ostream& os, const IRProgram& ir, const BinImage& bin_image, uint16_t address );
	void emit_mnemonic( std::ostream& os, const Opcode& opcode );
	void emit_operand( std::ostream& os, const IRProgram& ir, const Operand& op );
	void emit_element( std::ostream& os, const IRProgram& ir, const BinImage& bin_image, OutputMode mode, const InstructionElement& element );
	void emit_element( std::ostream& os, const IRProgram& ir, const BinImage& bin_image, OutputMode mode, const DataElement& element );
	void emit_header( std::ostream &os, const IRProgram& ir, std::string name );
};
