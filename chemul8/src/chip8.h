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

#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <map>

class Chemul8;

class Chip8
{
public:
	Chip8( Chemul8& hardware_ );

	void load_program( uint8_t program[], uint16_t program_size );
	void execute_instruction();

private:
	uint16_t Stack[16];
	uint8_t memory[4096];
	uint8_t V[16];
	uint16_t I = 0;
	uint16_t PC = 0x200;
	uint8_t SP = 0;

	const uint16_t font_sprite_base = 0x0100;
	const uint16_t program_base = 0x0200;
	Chemul8& hardware;

	typedef void (Chip8::*FN_DISP)( uint16_t );
	std::map<uint8_t,FN_DISP> dispatchers;

	void SYS( uint16_t opcode );		// 0x0nnn
	void JP( uint16_t opcode );			// 0x1nnn
	void CALL( uint16_t opcode );		// 0x2nnn
	void SEI( uint16_t opcode );		// 0x3xkk
	void SNI( uint16_t opcode );		// 0x4xkk
	void SER( uint16_t opcode );		// 0x5xyk
	void LD( uint16_t opcode );			// 0x6xkk
	void ADD( uint16_t opcode );		// 0x7xkk
	void MathOp( uint16_t opcode );		// 0x8xyn
	void SNE( uint16_t opcode );		// 0x9xy0
	void LDI( uint16_t opcode );		// 0xAnnn
	void JMP( uint16_t opcode );		// 0xBnnn
	void RND( uint16_t opcode );		// 0xCxkk
	void DRW( uint16_t opcode );		// 0xDxyn
	void Key( uint16_t opcode );		// 0xExkk
	void Misc( uint16_t opcode );		// 0xFxkk
};

#endif // CHIP8_H
