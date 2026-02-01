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

#include "cmdlineparser.h"

#include "chip8ir.h"

#include "targets.h"
#include <cassert>

class DisasmMemory
{
public:
    DisasmMemory( const BinImage& image, uint16_t origin = 0x200 ) : image_(image), origin( origin ), instruction_flag(image.size(), 0) {}

    void mark_instruction( uint16_t addr ) {
		assert( contains( addr ) );
		assert( contains( addr + 1 ) );

		instruction_flag[addr - origin] = true;
		instruction_flag[addr - origin + 1] = true;
	}

    uint8_t get_byte( uint16_t addr ) const { assert( contains( addr ) ); return image_[addr - origin]; };
	uint16_t get_word( uint16_t address) { return ( get_byte(address) << 8 ) | get_byte( address + 1 ); };

    uint16_t start() const { return origin; }
    uint16_t end() const { return origin + image_.size(); }

    bool is_instruction( uint16_t addr ) const { return contains( addr ) ? instruction_flag[addr - origin] : false; };

    bool contains( uint16_t addr ) const { return( (addr >= origin) && ( addr - origin < image_.size() ) ); }

private:
    uint16_t origin;
	const BinImage& image_;
    std::vector<uint8_t> instruction_flag;
};



class Disassembler
{
public:
	Disassembler() = default;

	void configure( const ChidasmCmdLineParser& cmd );

	IRProgram build_ir( const BinImage& binary );

private:
	ChidasmCmdLineParser cmd_;

	void collect_instructions( IRProgram& ir, DisasmMemory& memory, Targets& targets );
	void collect_data_bytes( IRProgram& ir, DisasmMemory& memory, Targets& targets );
};