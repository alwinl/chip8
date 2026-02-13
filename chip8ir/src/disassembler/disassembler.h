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

class DisasmMemory
{
public:
    DisasmMemory() = default;

	void bind( const BinImage& img, uint16_t start_at = 0x200 )
	{
		image_ = &img;
		origin = start_at;
		instruction_flag.assign( img.size(), false );
	}

	void unbind()
	{
		image_ = nullptr;
		origin = 0;
		instruction_flag.clear();
	}

    void mark_instruction( uint16_t addr )
	{
		assert( contains( addr ) );
		assert( contains( addr + 1 ) );

		instruction_flag[addr - origin] = true;
		instruction_flag[addr - origin + 1] = true;
	}

    uint8_t get_byte( uint16_t addr ) const { assert( contains( addr ) ); return (*image_)[addr - origin]; };
	uint16_t get_word( uint16_t address) { return ( get_byte(address) << 8 ) | get_byte( address + 1 ); };

    uint16_t start() const { return origin; }
    uint16_t end() const { return image_ ? origin + image_->size() : 0; }

    bool is_instruction( uint16_t addr ) const { return (contains( addr ) && contains( addr  + 1 )) ? instruction_flag[addr - origin] : false; };
	bool is_data( uint16_t addr ) const  { return contains( addr ) ? !instruction_flag[addr - origin] : false; };

    bool contains( uint16_t addr ) const { return( (addr >= origin) && ( addr - origin < image_->size() ) ); }

private:
    uint16_t origin = 0;
	const BinImage* image_ = nullptr;
    std::vector<uint8_t> instruction_flag;
};

class Disassembler
{
public:
	struct Config {
		uint16_t origin;
	};

	Disassembler() = default;

	void configure( Config config ) { configuration = std::move(config); };

	IRProgram build_ir( const BinImage& binary );

private:
	Config configuration;

	void collect_instructions( IRProgram& ir, DisasmMemory& memory );
	void collect_data_bytes( IRProgram& ir, DisasmMemory& memory );
};