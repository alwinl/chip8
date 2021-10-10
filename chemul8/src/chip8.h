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
 *
 */

#ifndef CHIP8_H
#define CHIP8_H

#include <iosfwd>

#include "display.h"
#include "keyboard.h"

class ResourceLayer;

class Chip8
{
public:
	Chip8( ResourceLayer& res_init );

	void load_program( std::istream& is );
	void run_program();

private:
	ResourceLayer& res;

	Display display;
	Keyboard keyboard;

	uint16_t Stack[16];
	uint8_t memory[4096];
	uint8_t V[16];
	uint16_t I;

	uint8_t DelayTimer;
	uint8_t SoundTimer;

	uint16_t PC;
	uint8_t SP;

	void decrease_timers();
	void execute_instruction();

	const uint16_t font_sprite_base = 0x0100;

	void SYS( uint16_t opcode );		// SYS addr
	void JP( uint16_t opcode );			// JP address
	void CALL( uint16_t opcode );		// CALL address
	void SEI( uint16_t opcode );		// SE Vx, byte
	void SNI( uint16_t opcode );		// SNE Vx, byte
	void SER( uint16_t opcode );		// SE Vx, Vy
	void LD( uint16_t opcode );			// LD, Vx, byte
	void ADD( uint16_t opcode );		// Add Vx, byte
	void MathOp( uint16_t opcode );		//
	void SNE( uint16_t opcode );		// SNE Vx, Vy
	void LDI( uint16_t opcode );		//0xAddd
	void JMP( uint16_t opcode );		//0xBddd
	void RND( uint16_t opcode );		//0xCddd
	void DRW( uint16_t opcode );	//0xDddd
	void Key( uint16_t opcode );		//0xEddd
	void Misc( uint16_t opcode );		//0xFddd
};

#endif // CHIP8_H
