/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
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
 *
 *	https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
 *
 */

#pragma once

#include <cstdint>
#include <map>

#include "quirks.h"

class Chip8
{
public:
	Chip8( Quirks::eChipType type, uint8_t * mem );

	void set_memory( uint8_t * mem );

	void execute_instruction();

private:
	uint8_t * memory;

	static constexpr uint16_t display_width = 64;
	static constexpr uint16_t display_height = 32;
	static constexpr uint16_t display_size = 64 * 32 / 8;
	static constexpr uint16_t display_base = 0x0F00;

	static constexpr uint16_t font_sprite_base = 0x0100;
	static constexpr uint16_t program_start = 0x0200;		// 512 bytes
	static constexpr uint16_t stack_start = 0x0EFF;			// 96 bytes (growing downwards)

	static constexpr uint8_t V_index = 0x00;			// 16 bytes
	static constexpr uint8_t I_index = 0x10;			// 2 bytes
	static constexpr uint8_t PC_index = 0x12;			// 2 bytes
	static constexpr uint8_t SP_index = 0x14;			// 2 bytes
	static constexpr uint8_t DT_index = 0x16;			// 1 byte
	static constexpr uint8_t ST_index = 0x17;			// 1 byte
	static constexpr uint8_t keys_index = 0x18;		// 2 bytes
	static constexpr uint8_t last_keys_index = 0x1A;	// 2 bytes
	static constexpr uint8_t int_index = 0x1C;			// 1 byte

	Quirks quirks;

	using Dispatcher = void ( Chip8::* )( uint16_t );
	using DispatcherMap = std::map<uint8_t, Dispatcher>;

	DispatcherMap dispatchers = {
		{ 0x00, &Chip8::SYS },	  { 0x01, &Chip8::JP },	 { 0x02, &Chip8::CALL }, { 0x03, &Chip8::SEI },
		{ 0x04, &Chip8::SNI },	  { 0x05, &Chip8::SER }, { 0x06, &Chip8::LD },	 { 0x07, &Chip8::ADD },
		{ 0x08, &Chip8::MathOp }, { 0x09, &Chip8::SNE }, { 0x0A, &Chip8::LDI },	 { 0x0B, &Chip8::JMP },
		{ 0x0C, &Chip8::RND },	  { 0x0D, &Chip8::DRW }, { 0x0E, &Chip8::Key },	 { 0x0F, &Chip8::Misc },

	};

	void SYS( uint16_t opcode );	// 0x0nnn
	void JP( uint16_t opcode );		// 0x1nnn
	void CALL( uint16_t opcode );	// 0x2nnn
	void SEI( uint16_t opcode );	// 0x3xkk
	void SNI( uint16_t opcode );	// 0x4xkk
	void SER( uint16_t opcode );	// 0x5xyk
	void LD( uint16_t opcode );		// 0x6xkk
	void ADD( uint16_t opcode );	// 0x7xkk
	void MathOp( uint16_t opcode ); // 0x8xyn
	void SNE( uint16_t opcode );	// 0x9xy0
	void LDI( uint16_t opcode );	// 0xAnnn
	void JMP( uint16_t opcode );	// 0xBnnn
	void RND( uint16_t opcode );	// 0xCxkk
	void DRW( uint16_t opcode );	// 0xDxyn
	void Key( uint16_t opcode );	// 0xExkk
	void Misc( uint16_t opcode );	// 0xFxkk

	void set_word( uint16_t base, uint16_t value );
	void set_PC( uint16_t value );
	void set_register( uint8_t index, uint8_t value );
	void set_I( uint16_t value );
	void stack_push( uint16_t value );
	void set_delay_timer( uint8_t value );
	void set_sound_timer( uint8_t value );

	uint16_t get_word( uint16_t base ) const;
	uint16_t get_PC() const;
	uint8_t get_register( uint8_t index ) const;
	uint16_t get_I();
	uint16_t stack_pop();
	uint8_t get_delay_timer() const;
	bool is_key_pressed( uint8_t key_no );
	bool key_captured( uint8_t &key_no );
	uint8_t get_random_value();

	void clear_screen();
	bool toggle_a_pixel( uint8_t x, uint8_t y );
};
