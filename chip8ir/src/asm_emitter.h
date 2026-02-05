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

#include "chip8ir.h"

class ASMEmitter
{
public:
	struct Config {
		std::string bin_name;
		bool is_clean;
	};

	ASMEmitter() = default;

	void emit( std::ostream& os, const IRProgram& ir );

	void configure( Config config ) { configuration = std::move(config); };

private:
	const IRProgram * program = nullptr;
	Config configuration;

	void emit_operand( std::ostream& os, const Operand& op );
	void emit_mnemonic( std::ostream& os, const Opcode& opcode );
	void emit_element( std::ostream& os, const InstructionElement& element );
	void emit_element( std::ostream& os, const DataElement& element );
	void emit_header( std::ostream &os );
};