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

#include <iostream>
#include <fstream>
#include <functional>
#include <random>

#include "chemul8.h"

Chip8::Chip8( Chemul8& hardware_ ) : hardware(hardware_)
{
	dispatchers.insert( std::make_pair(0x00, &Chip8::SYS) );
	dispatchers.insert( std::make_pair(0x01, &Chip8::JP) );
	dispatchers.insert( std::make_pair(0x02, &Chip8::CALL) );
	dispatchers.insert( std::make_pair(0x03, &Chip8::SEI) );
	dispatchers.insert( std::make_pair(0x04, &Chip8::SNI) );
	dispatchers.insert( std::make_pair(0x05, &Chip8::SER) );
	dispatchers.insert( std::make_pair(0x06, &Chip8::LD) );
	dispatchers.insert( std::make_pair(0x07, &Chip8::ADD) );
	dispatchers.insert( std::make_pair(0x08, &Chip8::MathOp) );
	dispatchers.insert( std::make_pair(0x09, &Chip8::SNE) );
	dispatchers.insert( std::make_pair(0x0A, &Chip8::LDI) );
	dispatchers.insert( std::make_pair(0x0B, &Chip8::JMP) );
	dispatchers.insert( std::make_pair(0x0C, &Chip8::RND) );
	dispatchers.insert( std::make_pair(0x0D, &Chip8::DRW) );
	dispatchers.insert( std::make_pair(0x0E, &Chip8::Key) );
	dispatchers.insert( std::make_pair(0x0F, &Chip8::Misc) );
}

void Chip8::load_program( std::istream& is )
{
	// reset the stack, registers, program counter, stack pointer and memory
	std::fill( std::begin(Stack), std::end(Stack), 0 );
	std::fill( std::begin(V), std::end(V), 0 );
	I = 0;
	PC = 0x200;
	SP = 0;
	std::fill( std::begin(memory), std::end(memory), 0 );

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

	//load the font in memory
	std::copy_n( &font[0], sizeof(font), &memory[font_sprite_base] );

	uint8_t ch =  is.get();

	for( uint16_t address = 0x200; is.good(); ++address ) {
		memory[address] = ch;
		ch = is.get();
	}
}

void Chip8::execute_instruction()
{
	uint16_t opcode = (memory[PC] << 8) | memory[PC+1];

	PC += 2;

	FN_DISP fp = dispatchers.at(opcode >> 12);
	(this->*fp)( opcode );
}

void Chip8::SYS( uint16_t opcode )	// 0nnn - SYS addr : Jump to a machine code routine at nnn.
{
	switch( opcode & 0xFFF ) {
	case 0x0E0:							// CLS : clear screen
		hardware.clear_screen();
		break;
	case 0x0EE:							// RET : return from subroutine
		PC = Stack[SP--];
		break;
	default:
		break;
	}
}

void Chip8::JP( uint16_t opcode )		// 1nnn - JP addr : Jump to location nnn.
{
	PC = opcode & 0xFFF;
}

void Chip8::CALL( uint16_t opcode )	// 2nnn - CALL addr : Call subroutine at nnn.
{
	++SP;
	Stack[SP] = PC;
	PC = opcode & 0xFFF;
}

void Chip8::SEI( uint16_t opcode )	// 3xkk - SE Vx, byte : Skip next instruction if Vx = kk.
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	if( V[reg_x] == (opcode & 0xFF) )
		PC += 2;
}

void Chip8::SNI( uint16_t opcode )	// 4xkk - SNE Vx, byte : Skip next instruction if Vx != kk.
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	if( V[reg_x] != (opcode & 0xFF) )
		PC += 2;
}

void Chip8::SER( uint16_t opcode )	// 5xy0 - SE Vx, Vy : Skip next instruction if Vx = Vy.
{
	uint8_t reg_x = (opcode >> 8) & 0xF;
	uint8_t reg_y = (opcode >> 4) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0:
		if( V[reg_x] == V[reg_y] )
			PC +=2;
		break;
	/* 0x1 .. 0xF not defined */
	default:
		break;
	}
}

void Chip8::LD( uint16_t opcode )		// 6xkk - LD Vx, byte : Set Vx = kk.
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	V[reg_x] = opcode & 0xFF;
}

void Chip8::ADD( uint16_t opcode )	// 7xkk - ADD Vx, byte : Set Vx = Vx + kk
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	V[reg_x] += opcode & 0xFF;
}

void Chip8::MathOp( uint16_t opcode )		// 8xyn - Various mathematical and logical operations
{
	uint8_t reg_x = (opcode >> 8) & 0xF;
	uint8_t reg_y = (opcode >> 4) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0: V[reg_x]  = V[reg_y]; 				  break;		// LD Vx, Vy : Set Vx = Vy.
	case 0x1: V[reg_x] |= V[reg_y]; vf_reset_quirk(); break;		// OR Vx, Vy : Set Vx = Vx OR Vy
	case 0x2: V[reg_x] &= V[reg_y]; vf_reset_quirk(); break;		// AND Vx, Vy : Set Vx = Vx AND Vy
	case 0x3: V[reg_x] ^= V[reg_y]; vf_reset_quirk(); break;		// XOR Vx, Vy : Set Vx = Vx XOR Vy
	case 0x4: {														// ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry
			uint16_t result = V[reg_x] + V[reg_y];
			V[reg_x] = result & 0xFF;
			V[0xF] = ( result > 255 ) ? 1 : 0;
		}
		break;
	case 0x5: {														// SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
			uint16_t result = (V[reg_x] >= V[reg_y]) ? 1 : 0;
			V[reg_x] -= V[reg_y];
			V[0xF] = result;
		}
		break;
	/*
	 * Note from Wikipedia:
	 *
	 * 	CHIP-8's opcodes 8XY6 and 8XYE (the bit shift instructions), which were in fact undocumented opcodes in the original interpreter,
	 *	shifted the value in the register VY and stored the result in VX. The CHIP-48 and SCHIP implementations instead ignored VY, and simply shifted VX
	 */
	case 0x6:	// SHR Vx {, Vy} : Set Vx = Vx SHR 1.	Error in documentation! should be Vx = Vy SHR 1
		{
			uint16_t result = V[reg_x] & 0x01;
			V[reg_x] = V[reg_y] >> 1;
			V[0xF] = result;
			break;
		}
	case 0x7: {										// SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
			uint16_t result = (V[reg_y] > V[reg_x]) ? 1 : 0;
			//V[reg_x] -= V[reg_y];
			V[reg_x] = V[reg_y] - V[reg_x];
			V[0xF] = result;
		}
		break;
	/* opcodes 0x8 .. 0xD not defined */
	case 0xE: {										// SHL Vx {, Vy} : Set Vx = Vx SHL 1. Error in documentation, should be Vx = Vy SHL 1
			uint16_t result = (V[reg_x] & 0x80) ? 1 : 0;
			V[reg_x] = V[reg_y] << 1;
			V[0xF] = result;
		}
		break;
	/* opcodes 0xF not defined */
	default:
		break;
	}
}

void Chip8::SNE( uint16_t opcode )		// 9xy0 - SNE Vx, Vy : Skip next instruction if Vx != Vy
{
	uint8_t reg_x = (opcode >> 8) & 0xF;
	uint8_t reg_y = (opcode >> 4) & 0xF;

	if( V[reg_x] != V[reg_y] )
		PC += 2;
}

void Chip8::LDI( uint16_t opcode )		// Annn - LD I, addr : Set I = nnn
{
	I = opcode & 0xFFF;
}

void Chip8::JMP( uint16_t opcode )		// Bnnn - JP V0, addr : Jump to location nnn + V0
{
	PC = (opcode & 0xFFF) + V[0];
}

void Chip8::RND( uint16_t opcode )		// Cxkk - RND Vx, byte : Set Vx = random byte AND kk
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	V[reg_x] = get_random_value() & (opcode & 0xFF );
}

void Chip8::DRW( uint16_t opcode )		// Dxyn - DRW Vx, Vy, nibble : Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
{
	uint8_t reg_x = (opcode >> 8) & 0xF;
	uint8_t reg_y = (opcode >> 4) & 0xF;

	V[0xF] = 0;
	uint8_t y = V[reg_y] % 32;

	if( ! int_set ) {
		PC -= 2;
		return;
	}

	for( uint8_t row = 0; row < (opcode & 0xF); ++row ) {

		uint8_t sprite_byte = memory[I + row];
		uint8_t x = V[reg_x] % 64;

		for( uint8_t bit_offset = 0; bit_offset < 8; ++bit_offset ) {
			if( sprite_byte & (1 << (7-bit_offset) ) )
				V[0x0F] |= hardware.toggle_a_pixel( x, y );

			++x;
			if( x == 64 )
				break;
		}
		++y;
		if( y == 32 )
			break;
	}
}

void Chip8::Key( uint16_t opcode )		//0xExkk
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	switch( opcode & 0xFF ) {
	/* opcodes 0x00 .. 0x9D not defined */
	case 0x9E:								// Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed
		if( hardware.is_key_pressed( V[reg_x] ) )
			PC += 2;
		break;
	/* opcodes 0x9F .. 0xA0 not defined */
	case 0xA1:								// ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
		if( ! hardware.is_key_pressed( V[reg_x] ) )
			PC += 2;
		break;
	/* opcodes 0xA2 .. 0xFF not defined */
	default:
		break;
	}
}

void Chip8::Misc( uint16_t opcode )		//0xFxkk
{
	uint8_t reg_x = (opcode >> 8) & 0xF;

	switch( opcode & 0xFF ) {
	/* opcodes 0x00 .. 0x06 not defined */
	case 0x07: V[reg_x] = hardware.get_delay_timer(); break;	// Fx07 - LD Vx, DT : Set Vx = delay timer value.
	/* opcodes 0x08 .. 0x09 not defined */
	case 0x0A:
		{
			uint8_t key_no;
			if( hardware.key_captured( key_no ) )
				V[reg_x] = key_no;
			else
				PC -= 2;
		}
		break;

		//get_key( reg_x ); break;						// Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
	/* opcodes 0x0B .. 0x1 not defined */
	case 0x15: hardware.set_delay_timer( V[reg_x] ); break;	// Fx15 - LD DT, Vx : Set delay timer = Vx.
	/* opcodes 0x16 and 0x17 not defined */
	case 0x18: hardware.set_sound_timer( V[reg_x] ); break;	// Fx18 - LD ST, Vx : Set sound timer = Vx.
	/* opcodes 0x19 .. 0x1D not defined */
	case 0x1E: I += V[reg_x]; break;						// Fx1E - ADD I, Vx : Set I = I + Vx
	/* opcodes 0x1F .. 0x28 not defined */
	case 0x29: I = font_sprite_base + 5 * V[reg_x]; break;	// Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
	/* opcodes 0x2A .. 0x32 not defined */
	case 0x33 : 											// Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2
		memory[I] = V[reg_x] / 100;
		memory[I+1] = (V[reg_x] / 10) % 10;
		memory[I+2] = V[reg_x] % 10;
		break;
	/* opcodes 0x34 .. 0x54 not defined */
	case 0x55: {											// Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I
			int i;
			for( i=0; i<=reg_x; ++i )
				memory[ I + i] = V[i];
			memory_quirk( i );
		}
		break;
	/* opcodes 0x56 .. 0x64 not defined */
	case 0x65: {											// Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I
			int i;
			for( i=0; i<=reg_x; ++i )
				V[i] = memory[ I + i];
			memory_quirk( i );
		}
		break;
	/* opcodes 0x66 .. 0xFF not defined */
	default:
		break;
	}
}

uint8_t Chip8::get_random_value()
{
	static std::mt19937 mt{ std::random_device{}() };

	std::uniform_int_distribution<> dist(1,255);
	return dist( mt );
}


void Chip8::vf_reset_quirk()
{
	V[0x0F] = 0;
}

void Chip8::memory_quirk( int bytes_to_add )
{
	I += bytes_to_add;
}


