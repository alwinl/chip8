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
	void DRW( uint16_t opcode );		//0xDddd
	void Key( uint16_t opcode );		//0xEddd
	void Misc( uint16_t opcode );		//0xFddd

	Display& display;
	Keyboard& keyboard;
	Timers& timers;

	void vf_reset_quirk();
	void memory_quirk( int bytes_to_add );

	class Chip8_KeyTrigger : KeyTrigger
	{
	public:
		Chip8_KeyTrigger( Keyboard& keyboard_, Chip8 * target_ ) : KeyTrigger( keyboard_ ), target( target_ ) {};
		virtual ~Chip8_KeyTrigger() {};

		void set_waiting_register( uint8_t reg ) { this->reg = reg; };

		virtual void key_captured( uint8_t key_value ) { target->key_captured( reg, key_value ); };

	private:
		Chip8 * target;
		uint8_t reg;

	};

	friend class Chip8_KeyTrigger;
	void key_captured( uint8_t reg_x, uint8_t key_value )  { V[reg_x] = key_value; }

	Chip8_KeyTrigger key_trigger;

	void get_key( uint8_t reg )
	{
		key_trigger.set_waiting_register( reg );
		keyboard.wait_for_key( );
	}

	uint8_t get_random_value()
	{
		static std::mt19937 mt{ std::random_device{}() };

		std::uniform_int_distribution<> dist(1,255);
		return dist( mt );
	}
};

#endif // CHIP8_H
