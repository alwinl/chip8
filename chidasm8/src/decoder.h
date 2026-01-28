/*
 * decoder.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <array>
#include <optional>

#include "instruction.h"
#include "targets.h"

using AddressList = std::vector<uint16_t>;

class Memory;

struct DecodeResult
{
	Instruction instruction;
	AddressList next_addresses;
	std::optional<DecodedTarget> target;
};

class Decoder
{
public:
	Decoder() = default;

	DecodeResult decode(uint16_t address, Memory& memory );

private:
	using Dispatcher = std::array<DecodeResult (Decoder::*)(uint16_t address, uint16_t opcode), 16>;
	static Dispatcher dispatcher;

	DecodeResult decode_SYS( uint16_t address, uint16_t opcode );
	DecodeResult decode_JP( uint16_t address, uint16_t opcode );
	DecodeResult decode_CALL( uint16_t address, uint16_t opcode );
	DecodeResult decode_SEI( uint16_t address, uint16_t opcode );
	DecodeResult decode_SNEI( uint16_t address, uint16_t opcode );
	DecodeResult decode_SER( uint16_t address, uint16_t opcode );
	DecodeResult decode_LD( uint16_t address, uint16_t opcode );
	DecodeResult decode_ADD( uint16_t address, uint16_t opcode );
	DecodeResult decode_MathOp( uint16_t address, uint16_t opcode );
	DecodeResult decode_SNE( uint16_t address, uint16_t opcode );
	DecodeResult decode_LDI( uint16_t address, uint16_t opcode );
	DecodeResult decode_JMP( uint16_t address, uint16_t opcode );
	DecodeResult decode_RND( uint16_t address, uint16_t opcode );
	DecodeResult decode_DRW( uint16_t address, uint16_t opcode );
	DecodeResult decode_Key( uint16_t address, uint16_t opcode );
	DecodeResult decode_Misc( uint16_t address, uint16_t opcode );
};
