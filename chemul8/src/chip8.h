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
#include <random>

#include "display.h"
#include "keyboard.h"
#include "timers.h"

class Chip8_KeyTrigger : public KeyTrigger
{
public:
	Chip8_KeyTrigger( Keyboard& keyboard ) : KeyTrigger( keyboard ) {};
	virtual ~Chip8_KeyTrigger() {};

	void set_waiting_register( uint8_t * reg_loc ) { this->reg_loc = reg_loc; start_waiting(); };
	virtual void key_captured( uint8_t key_value ) { *reg_loc = key_value; };

private:
	uint8_t * reg_loc;
};


class Chip8
{
public:
	Chip8( Display& display_, Keyboard& keyboard_, Timers& timers_ );

	void load_program( std::istream& is );
	void execute_instruction();

private:
	uint16_t Stack[16];
	uint8_t memory[4096];
	uint8_t V[16];
	uint16_t I = 0;
	uint16_t PC = 0x200;
	uint8_t SP = 0;

	Display& display;
	Timers& timers;
	Chip8_KeyTrigger key_trigger;

	const uint16_t font_sprite_base = 0x0100;

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

	void get_key( uint8_t reg ) { key_trigger.set_waiting_register( &V[reg] ); }
	bool is_key_pressed( uint8_t reg ) { return key_trigger.is_key_pressed( reg ); }

	uint8_t get_random_value();

	void vf_reset_quirk();
	void memory_quirk( int bytes_to_add );
};

#endif // CHIP8_H
