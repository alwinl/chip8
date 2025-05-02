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
#include <cstring>

#include "chemul8.h"

Chip8::Chip8( Chemul8 &hardware_, Quirks::eChipType type ) : hardware( hardware_ ), quirks( type ) { }

void Chip8::set_memory( uint8_t *mem )
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
	
	memory = mem;

	std::fill( std::begin( Stack ), std::end( Stack ), 0 );
	std::fill( std::begin( V ), std::end( V ), 0 );
	I = 0;
	SP = 0;

	// load font and program code in memory
	std::copy_n( &font[0], sizeof( font ), &memory[font_sprite_base] );

	memory[PC_index] = 0x0200 >> 8;
	memory[PC_index + 1] = 0x0200 & 0xFF;
}

uint16_t Chip8::get_PC() const
{
	return ( memory[PC_index] << 8 ) | memory[PC_index + 1];
}

void Chip8::set_PC( uint16_t value )
{
	memory[PC_index] = value >> 8;
	memory[PC_index + 1] = value & 0xFF;
}


void Chip8::clear_screen()
{
	std::memset(&memory[display_base::value], 0, display_size::value);
}

bool Chip8::toggle_a_pixel( uint8_t x, uint8_t y )
{
	// using display_width = std::integral_constant<uint16_t, 64>;
	// using display_height = std::integral_constant<uint16_t, 32>;
	// using display_size = std::integral_constant<uint16_t, 64 * 32 / 8>;
	// using display_base = std::integral_constant<uint16_t, 0x0F00>;

	// uint16_t byte_index = ( x + y * display_width::value ) / 8;
	// uint8_t bit_offset = ( x + y * display_width::value ) % 8;

	// if( byte_index >= display_size::value )
	// 	return false;

	// bool turned_off = ( memory[display_base::value + byte_index] & ( 1 << bit_offset ) );
	// memory[display_base::value + byte_index] ^= ( 1 << bit_offset );

	// return turned_off;
	
	return hardware.toggle_a_pixel( x, y );
}
bool Chip8::is_key_pressed( uint8_t key_no )
{
	return hardware.is_key_pressed( key_no );
}
bool Chip8::key_captured( uint8_t &key_no )
{
	// uint16_t key_changes = keys ^ last_keys;

	// if( !key_changes )
	// 	return false;

	// last_keys = keys;

	// for( key_no = 0; key_no < 16; ++key_no )
	// 	if( ( ( key_changes >> key_no ) & 0x01 ) && ( !( ( keys >> key_no ) & 0x01 ) ) )
	// 		return true;

	// return false;

	return hardware.key_captured( key_no );
}
void Chip8::set_delay_timer( uint8_t value )
{
	hardware.set_delay_timer( value );
}
void Chip8::set_sound_timer( uint8_t value )
{
	hardware.set_sound_timer( value );
}
uint8_t Chip8::get_delay_timer() const
{
	return hardware.get_delay_timer();
}
uint8_t Chip8::get_random_value()
{
	return hardware.get_random_value();
}

void Chip8::execute_instruction( bool tick )
{
	interrupt = tick;
	// if( interrupt ) {
	// 	if( DelayTimer > 0 )
	// 		--DelayTimer;

	// 	if( SoundTimer > 0 ) {
	// 		if( SoundTimer == 1 )
	// 			hardware.set_sound_timer( 0 );
	// 		--SoundTimer;
	// 	}
	// }

	const uint16_t opcode = ( memory[get_PC()] << 8 ) | memory[get_PC() + 1];

	set_PC( get_PC() + 2 );

	auto fp_dispatch = dispatchers.at( opcode >> 12 );
	( this->*fp_dispatch )( opcode );
}

void Chip8::SYS( uint16_t opcode ) // 0nnn - SYS addr : Jump to a machine code routine at nnn.
{
	switch( opcode & 0xFFF ) {
	case 0x0E0: // CLS : clear screen
		clear_screen();
		break;

	case 0x0EE: // RET : return from subroutine
		set_PC( Stack[SP--] );
		break;

	default: set_PC( opcode & 0xFFF ); break;
	}
}

void Chip8::JP( uint16_t opcode ) // 1nnn - JP addr : Jump to location nnn.
{
	set_PC( opcode & 0xFFF );
}

void Chip8::CALL( uint16_t opcode ) // 2nnn - CALL addr : Call subroutine at nnn.
{
	++SP;
	Stack[SP] = get_PC();
	set_PC( opcode & 0xFFF );
}

void Chip8::SEI( uint16_t opcode ) // 3xkk - SE Vx, byte : Skip next instruction if Vx = kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	if( V[reg_x] == ( opcode & 0xFF ) )
		set_PC( get_PC() + 2 );
}

void Chip8::SNI( uint16_t opcode ) // 4xkk - SNE Vx, byte : Skip next instruction if Vx != kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	if( V[reg_x] != ( opcode & 0xFF ) )
		set_PC( get_PC() + 2 );
}

void Chip8::SER( uint16_t opcode ) // 5xy0 - SE Vx, Vy : Skip next instruction if Vx = Vy.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0:
		if( V[reg_x] == V[reg_y] )
			set_PC( get_PC() + 2 );
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
			set_PC( get_PC() + 2 );
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
		set_PC( ( opcode & 0xFFF ) + V[( opcode >> 8 ) & 0x0F] );
	else
		set_PC( ( opcode & 0xFFF ) + V[0] );
}

void Chip8::RND( uint16_t opcode ) // Cxkk - RND Vx, byte : Set Vx = random byte AND kk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	V[reg_x] = get_random_value() & ( opcode & 0xFF );
}

void Chip8::DRW( uint16_t opcode ) // Dxyn - DRW Vx, Vy, nibble : Display n-byte sprite starting at memory location I at
								   // (Vx, Vy), set VF = collision
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	V[0xF] = 0;
	uint8_t ypos = V[reg_y] % 32;

	if( quirks.has_quirk( Quirks::eQuirks::DISP_WAIT ) && !interrupt ) { // rate limit the DRW calls to 60fps
		set_PC( get_PC() - 2 );
		return;
	}

	const uint8_t end_row = opcode & 0xF;
	for( uint8_t row = 0; row < end_row; ++row ) {

		const uint8_t sprite_byte = memory[I + row];
		uint8_t xpos = V[reg_x] % 64;

		for( uint8_t bit_offset = 0; bit_offset < 8; ++bit_offset ) {
			if( sprite_byte & ( 1 << ( 7 - bit_offset ) ) )
				V[0x0F] |= toggle_a_pixel( xpos % 64, ypos % 32 );

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
		if( is_key_pressed( V[reg_x] ) )
			set_PC( get_PC() + 2 );
		break;
		// opcodes 0x9F .. 0xA0 not defined
	case 0xA1: // ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
		if( !is_key_pressed( V[reg_x] ) )
			set_PC( get_PC() + 2 );
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
		V[reg_x] = get_delay_timer();
		break;
		// opcodes 0x08 .. 0x09 not defined
	case 0x0A: // Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
	{
		uint8_t key_no = 0;
		if( key_captured( key_no ) )
			V[reg_x] = key_no;
		else
		set_PC( get_PC() - 2 );
	} break;
		// opcodes 0x0B .. 0x1 not defined
	case 0x15: // Fx15 - LD DT, Vx : Set delay timer = Vx.
		set_delay_timer( V[reg_x] );
		break;
		// opcodes 0x16 and 0x17 not defined
	case 0x18: // Fx18 - LD ST, Vx : Set sound timer = Vx.
		set_sound_timer( V[reg_x] );
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