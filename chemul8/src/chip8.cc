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

#include "chip8.h"

#include <algorithm>
#include <functional>

#include "chemul8.h"

Chip8::Chip8( Chemul8 &hardware_, Quirks::eChipType type ) : hardware( hardware_ ), quirks( type ) { }

void Chip8::load_program( uint8_t program[], uint16_t program_size )
{
	static uint8_t font[] = {
		/* 0 */ 0xF0, 0x90, 0x90, 0x90, 0xF0,
		/* 1 */ 0x20, 0x60, 0x20, 0x20, 0x70,
		/* 2 */ 0xF0, 0x10, 0xF0, 0x80, 0xf0,
		/* 3 */ 0xF0, 0x10, 0xF0, 0x10, 0xF0,
		/* 4 */ 0x90, 0x90, 0xF0, 0x10, 0x10,
		/* 5 */ 0xF0, 0x80, 0xF0, 0x10, 0xF0,
		/* 6 */ 0xF0, 0x80, 0xF0, 0x90, 0xF0,
		/* 7 */ 0xF0, 0x10, 0x20, 0x40, 0x40,
		/* 8 */ 0xF0, 0x90, 0xF0, 0x90, 0xF0,
		/* 9 */ 0xF0, 0x90, 0xF0, 0x10, 0xF0,
		/* A */ 0xF0, 0x90, 0xF0, 0x90, 0x90,
		/* B */ 0xE0, 0x90, 0xE0, 0x90, 0xE0,
		/* C */ 0xF0, 0x80, 0x80, 0x80, 0xF0,
		/* D */ 0xE0, 0x90, 0x90, 0x90, 0xE0,
		/* E */ 0xF0, 0x80, 0xF0, 0x80, 0xF0,
		/* F */ 0xF0, 0x80, 0xF0, 0x80, 0x80,
	};

	// reset the stack, registers, program counter, stack pointer and memory
	std::fill( std::begin( Stack ), std::end( Stack ), 0 );
	std::fill( std::begin( V ), std::end( V ), 0 );
	I = 0;
	PC = 0x200;
	SP = 0;
	std::fill( std::begin( memory ), std::end( memory ), 0 );

	// load font and program code in memory
	std::copy_n( &font[0], sizeof( font ), &memory[font_sprite_base] );
	std::copy_n( &program[0], program_size, &memory[program_base] );
}

void Chip8::execute_instruction()
{
	const uint16_t opcode = ( memory[PC] << 8 ) | memory[PC + 1];

	PC += 2;

	auto fp_dispatch = dispatchers.at( opcode >> 12 );
	( this->*fp_dispatch )( opcode );
}

void Chip8::SYS( uint16_t opcode ) // 0nnn - SYS addr : Jump to a machine code routine at nnn.
{
	switch( opcode & 0xFFF ) {
	case 0x0E0: // CLS : clear screen
		hardware.clear_screen();
		break;

	case 0x0EE: // RET : return from subroutine
		PC = Stack[SP--];
		break;

	default: PC = opcode & 0xFFF; break;
	}
}

void Chip8::JP( uint16_t opcode ) // 1nnn - JP addr : Jump to location nnn.
{
	PC = opcode & 0xFFF;
}

void Chip8::CALL( uint16_t opcode ) // 2nnn - CALL addr : Call subroutine at nnn.
{
	++SP;
	Stack[SP] = PC;
	PC = opcode & 0xFFF;
}

void Chip8::SEI( uint16_t opcode ) // 3xkk - SE Vx, byte : Skip next instruction if Vx = kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	if( V[reg_x] == ( opcode & 0xFF ) )
		PC += 2;
}

void Chip8::SNI( uint16_t opcode ) // 4xkk - SNE Vx, byte : Skip next instruction if Vx != kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	if( V[reg_x] != ( opcode & 0xFF ) )
		PC += 2;
}

void Chip8::SER( uint16_t opcode ) // 5xy0 - SE Vx, Vy : Skip next instruction if Vx = Vy.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0:
		if( V[reg_x] == V[reg_y] )
			PC += 2;
		break;
		// 0x1 .. 0xF not defined
	default: break;
	}
}

void Chip8::LD( uint16_t opcode ) // 6xkk - LD Vx, byte : Set Vx = kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	V[reg_x] = opcode & 0xFF;
}

void Chip8::ADD( uint16_t opcode ) // 7xkk - ADD Vx, byte : Set Vx = Vx + kk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	V[reg_x] += opcode & 0xFF;
}

void Chip8::MathOp( uint16_t opcode ) // 8xyn - Various mathematical and logical operations
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0: // LD Vx, Vy : Set Vx = Vy.
		V[reg_x] = V[reg_y];
		break;

	case 0x1: // OR Vx, Vy : Set Vx = Vx OR Vy
		V[reg_x] |= V[reg_y];
		if( quirks.has_quirk( Quirks::eQuirks::RESET ) )
			V[0x0F] = 0;
		break;

	case 0x2: // AND Vx, Vy : Set Vx = Vx AND Vy
		V[reg_x] &= V[reg_y];
		if( quirks.has_quirk( Quirks::eQuirks::RESET ) )
			V[0x0F] = 0;
		break;

	case 0x3: // XOR Vx, Vy : Set Vx = Vx XOR Vy
		V[reg_x] ^= V[reg_y];
		if( quirks.has_quirk( Quirks::eQuirks::RESET ) )
			V[0x0F] = 0;
		break;

	case 0x4: // ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry
	{
		const uint16_t result = V[reg_x] + V[reg_y];
		V[reg_x] = result & 0xFF;
		V[0xF] = ( result > 255 ) ? 1 : 0;
	} break;

	case 0x5: // SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
	{
		const uint16_t result = ( V[reg_x] >= V[reg_y] ) ? 1 : 0;
		V[reg_x] -= V[reg_y];
		V[0xF] = result;
	} break;

	case 0x6: // SHR Vx {, Vy} : Set Vx = Vx SHR 1 or Vx = Vy SHR 1
	{
		const uint16_t result = V[reg_x] & 0x01;
		if( quirks.has_quirk( Quirks::eQuirks::SHIFTING ) )
			V[reg_x] = V[reg_x] >> 1;
		else
			V[reg_x] = V[reg_y] >> 1;
		V[0xF] = result;
	} break;

	case 0x7: // SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
	{
		const uint16_t result = ( V[reg_y] > V[reg_x] ) ? 1 : 0;
		V[reg_x] = V[reg_y] - V[reg_x];
		V[0xF] = result;
	} break;
		// opcodes 0x8 .. 0xD not defined
	case 0xE: // SHL Vx {, Vy} : Set Vx = Vx SHL 1 or Vx = Vy SHL 1
	{
		const uint16_t result = ( V[reg_x] & 0x80 ) ? 1 : 0;
		if( quirks.has_quirk( Quirks::eQuirks::SHIFTING ) )
			V[reg_x] = V[reg_x] << 1;
		else
			V[reg_x] = V[reg_y] << 1;
		V[0xF] = result;
	} break;
		// opcodes 0xF not defined
	default: break;
	}
}

void Chip8::SNE( uint16_t opcode ) // 9xy0 - SNE Vx, Vy : Skip next instruction if Vx != Vy
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0:
		if( V[reg_x] != V[reg_y] )
			PC += 2;
		break;

	// opcodes 0x1 to 0xF not defined
	default: break;
	}
}

void Chip8::LDI( uint16_t opcode ) // Annn - LD I, addr : Set I = nnn
{
	I = opcode & 0xFFF;
}

void Chip8::JMP(
	uint16_t opcode ) // Bnnn - JP V0, addr : Jump to location nnn + V0 or Bxnn : Jump to location nn + V[x]
{
	if( quirks.has_quirk( Quirks::eQuirks::JUMPING ) )
		PC = ( opcode & 0xFFF ) + V[( opcode >> 8 ) & 0x0F];
	else
		PC = ( opcode & 0xFFF ) + V[0];
}

void Chip8::RND( uint16_t opcode ) // Cxkk - RND Vx, byte : Set Vx = random byte AND kk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	V[reg_x] = hardware.get_random_value() & ( opcode & 0xFF );
}

void Chip8::DRW( uint16_t opcode ) // Dxyn - DRW Vx, Vy, nibble : Display n-byte sprite starting at memory location I at
								   // (Vx, Vy), set VF = collision
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	V[0xF] = 0;
	uint8_t ypos = V[reg_y] % 32;

	if( quirks.has_quirk( Quirks::eQuirks::DISP_WAIT ) && hardware.block_drw() ) { // rate limit the DRW calls to 60fps
		PC -= 2;
		return;
	}

	const uint8_t end_row = opcode & 0xF;
	for( uint8_t row = 0; row < end_row; ++row ) {

		const uint8_t sprite_byte = memory[I + row];
		uint8_t xpos = V[reg_x] % 64;

		for( uint8_t bit_offset = 0; bit_offset < 8; ++bit_offset ) {
			if( sprite_byte & ( 1 << ( 7 - bit_offset ) ) )
				V[0x0F] |= hardware.toggle_a_pixel( xpos % 64, ypos % 32 );

			++xpos;

			if( quirks.has_quirk( Quirks::eQuirks::CLIPPING ) && ( xpos == 64 ) )
				break;
		}

		++ypos;

		if( quirks.has_quirk( Quirks::eQuirks::CLIPPING ) && ( ypos == 32 ) )
			break;
	}
}

void Chip8::Key( uint16_t opcode ) // 0xExkk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	switch( opcode & 0xFF ) {
		// opcodes 0x00 .. 0x9D not defined
	case 0x9E: // Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed
		if( hardware.is_key_pressed( V[reg_x] ) )
			PC += 2;
		break;
		// opcodes 0x9F .. 0xA0 not defined
	case 0xA1: // ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
		if( !hardware.is_key_pressed( V[reg_x] ) )
			PC += 2;
		break;
		// opcodes 0xA2 .. 0xFF not defined
	default: break;
	}
}

void Chip8::Misc( uint16_t opcode ) // 0xFxkk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	switch( opcode & 0xFF ) {
		// opcodes 0x00 .. 0x06 not defined
	case 0x07: // Fx07 - LD Vx, DT : Set Vx = delay timer value.
		V[reg_x] = hardware.get_delay_timer();
		break;
		// opcodes 0x08 .. 0x09 not defined
	case 0x0A: // Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
	{
		uint8_t key_no = 0;
		if( hardware.key_captured( key_no ) )
			V[reg_x] = key_no;
		else
			PC -= 2;
	} break;
		// opcodes 0x0B .. 0x1 not defined
	case 0x15: // Fx15 - LD DT, Vx : Set delay timer = Vx.
		hardware.set_delay_timer( V[reg_x] );
		break;
		// opcodes 0x16 and 0x17 not defined
	case 0x18: // Fx18 - LD ST, Vx : Set sound timer = Vx.
		hardware.set_sound_timer( V[reg_x] );
		break;
		// opcodes 0x19 .. 0x1D not defined
	case 0x1E: // Fx1E - ADD I, Vx : Set I = I + Vx
		I += V[reg_x];
		break;
		// opcodes 0x1F .. 0x28 not defined
	case 0x29: // Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
		I = font_sprite_base + 5 * V[reg_x];
		break;
		// opcodes 0x2A .. 0x32 not defined
	case 0x33: // Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2
		memory[I] = V[reg_x] / 100;
		memory[I + 1] = ( V[reg_x] / 10 ) % 10;
		memory[I + 2] = V[reg_x] % 10;
		break;
		// opcodes 0x34 .. 0x54 not defined
	case 0x55: // Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I
	{
		int idx = 0;
		for( ; idx <= reg_x; ++idx ) memory[I + idx] = V[idx];
		if( quirks.has_quirk( Quirks::eQuirks::MEMORY ) )
			I += idx;
	} break;
		// opcodes 0x56 .. 0x64 not defined
	case 0x65: // Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I
	{
		int idx = 0;
		for( ; idx <= reg_x; ++idx ) V[idx] = memory[I + idx];
		if( quirks.has_quirk( Quirks::eQuirks::MEMORY ) )
			I += idx;
	} break;
		// opcodes 0x66 .. 0xFF not defined
	default: break;
	}
}