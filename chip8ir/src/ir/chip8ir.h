/*
 * chip8ir.h Copyright 2026 Alwin Leerling <dna.leerling@gmail.com>
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

#include <cstdint>
#include <cstddef>
#include <variant>
#include <span>
#include <cassert>
#include <ostream>
#include <algorithm>
#include <vector>
#include <optional>

#include "chip8instruction.h"

struct IRDataBlock
{
	uint16_t address;
	std::vector<uint8_t> byte_run;
};

struct IRInstruction
{
	uint16_t address;
	Instruction instruction;
};

using IRElement = std::variant<IRInstruction, IRDataBlock>;

struct IRProgram
{
    uint16_t origin = 0x200;
    std::vector<IRElement> elements;
};

bool operator==( const IRProgram& a, const IRProgram& b );
bool operator==( const IRElement& a, const IRElement& b );
bool operator==( const IRDataBlock& a, const IRDataBlock& b );
bool operator==( const IRInstruction& a, const IRInstruction& b );

std::ostream& operator<<(std::ostream& os, const IRProgram& ir);
std::ostream& operator<<(std::ostream& os, const IRDataBlock& data_element);
std::ostream& operator<<(std::ostream& os, const IRInstruction& instr_element);
std::ostream& operator<<(std::ostream& os, const Instruction& instr);
