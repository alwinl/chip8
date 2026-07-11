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
 */

#include "chip8.h"

#include <algorithm>
#include <functional>
#include <cstring>
#include <random>

/*
	memory layout:

	0x000–0x01F   CPU state (registers, I, PC, SP, timers, keys, quirks)  [32 bytes]
	0x020–0x06F   Font sprites (80 bytes)
	0x070–0x09F   Spare / interpreter scratch (48 bytes)
	0x0A0–0x0FF   Stack (96 bytes, grows downward)
	0x100–0x1FF   Display buffer (256 bytes)
	0x200–0xFFF   Program + data

*/
constexpr uint8_t QUIRK_RESET 		= 1 << 0;
constexpr uint8_t QUIRK_MEMORY		= 1 << 1;
constexpr uint8_t QUIRK_DISP_WAIT	= 1 << 2;
constexpr uint8_t QUIRK_CLIPPING	= 1 << 3;
constexpr uint8_t QUIRK_SHIFTING	= 1 << 4;
constexpr uint8_t QUIRK_JUMPING		= 1 << 5;

void Chip8::set_quirk_type( eQuirkType type )
{
	switch( type ) {
	case eQuirkType::CHIP8: memory[Quirk_index] = QUIRK_RESET | QUIRK_MEMORY | QUIRK_DISP_WAIT| QUIRK_CLIPPING; break;
	case eQuirkType::XOCHIP : memory[Quirk_index] = QUIRK_MEMORY; break;
	case eQuirkType::SCHIP : memory[Quirk_index] = QUIRK_CLIPPING | QUIRK_SHIFTING | QUIRK_JUMPING; break;
	}
}

void Chip8::set_program( uint8_t *mem, size_t size )
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

	memset( memory, 0, sizeof( memory ) );

	std::copy_n( mem, size, &memory[program_start] );
	std::copy_n( &font[0], sizeof( font ), &memory[font_sprite_base] );

	set_word( PC_index, program_start );
	set_word( SP_index, stack_start );
}

uint16_t Chip8::get_word( uint16_t base ) const
{
	return (memory[base] << 8 ) | memory[base + 1];
}

void Chip8::set_word( uint16_t base, uint16_t value )
{
	memory[base] = value >> 8;
	memory[base + 1] = value & 0xFF;
}

uint16_t Chip8::get_PC() const
{
	return get_word( PC_index );
}

void Chip8::set_PC( uint16_t value )
{
	set_word( PC_index, value );
}

uint8_t Chip8::get_register( uint8_t index ) const
{
	return memory[V_index + index ];
}

void Chip8::set_register( uint8_t index, uint8_t value )
{
	memory[V_index + index] = value;
}

void Chip8::set_I( uint16_t value )
{
	set_word( I_index, value );
}

uint16_t Chip8::get_I(  )
{
	return get_word( I_index );
}

void Chip8::stack_push( uint16_t value )
{
	uint16_t address = get_word( SP_index );

	address -= 2;
	set_word( address, value );

	set_word( SP_index, address );
}

uint16_t Chip8::stack_pop()
{
	uint16_t address = get_word( SP_index );

	uint16_t value = get_word( address );
	address += 2;

	set_word( SP_index, address );

	return value;
}

void Chip8::clear_screen()
{
	std::memset(&memory[display_base], 0, display_size);
}

bool Chip8::toggle_a_pixel( uint8_t x, uint8_t y )
{
	uint16_t byte_index = ( x + y * display_width ) / 8;
	uint8_t bit_offset = ( x + y * display_width ) % 8;

	if( byte_index >= display_size )
		return false;

	bool turned_off = ( memory[display_base + byte_index] & ( 1 << bit_offset ) );
	memory[display_base + byte_index] ^= ( 1 << bit_offset );

	return turned_off;
}

bool Chip8::is_key_pressed( uint8_t key_no )
{
	return ( get_word( keys_index ) >> key_no ) & 0x01;
}

bool Chip8::key_captured( uint8_t &key_no )
{
	uint16_t keys = get_word( keys_index );
	uint16_t key_changes = keys ^ get_word( last_keys_index );

	if( !key_changes )
		return false;

	set_word( last_keys_index, keys );

	for( key_no = 0; key_no < 16; ++key_no )
		if( ( ( key_changes >> key_no ) & 0x01 ) && ( !( ( keys >> key_no ) & 0x01 ) ) )
			return true;

	return false;
}

void Chip8::set_interrupt( bool on )
{
	memory[int_index] = on ? 1 : 0;
}

void Chip8::set_keys_state( uint16_t new_state )
{
	set_word( keys_index, new_state);
}

void Chip8::set_delay_timer( uint8_t value )
{
	memory[DT_index] = value;
}

void Chip8::set_sound_timer( uint8_t value )
{
	memory[ST_index] = value;
}

uint8_t Chip8::get_delay_timer() const
{
	return memory[DT_index];
}

void Chip8::decrease_timers()
{
	if( memory[DT_index] > 0 )
		--memory[DT_index];

	if( memory[ST_index] > 0 )
		--memory[ST_index];
}


uint8_t Chip8::get_random_value()
{
	static std::mt19937 mt{ std::random_device{}() };

	std::uniform_int_distribution<> dist( 1, 255 );
	return dist( mt );
}

void Chip8::clock_tick( )
{
	const uint16_t opcode = get_word( get_PC() );

	set_PC( get_PC() + 2 );

	auto fp_dispatch = dispatchers.at( opcode >> 12 );
	( this->*fp_dispatch )( opcode );
}

void Chip8::SYS( uint16_t opcode ) // 0nnn - SYS addr : Jump to a machine code routine at nnn.
{
	const uint16_t address = opcode & 0xFFF;

	switch( address ) {
	case 0x0E0: // CLS : clear screen
		clear_screen();
		break;

	case 0x0EE: // RET : return from subroutine
		set_PC( stack_pop() );
		// set_PC( Stack[SP--] );
		break;

	default:
		set_PC( address );
		break;
	}
}

void Chip8::JP( uint16_t opcode ) // 1nnn - JP addr : Jump to location nnn.
{
	const uint16_t address = opcode & 0xFFF;

	set_PC( address );
}

void Chip8::CALL( uint16_t opcode ) // 2nnn - CALL addr : Call subroutine at nnn.
{
	const uint16_t address = opcode & 0xFFF;

	stack_push( get_PC() );
	// ++SP;
	// Stack[SP] = get_PC();
	set_PC( address );
}

void Chip8::SEI( uint16_t opcode ) // 3xkk - SE Vx, byte : Skip next instruction if Vx = kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t immediate = opcode & 0xFF;

	if( get_register( reg_x ) == immediate )
		set_PC( get_PC() + 2 );
}

void Chip8::SNI( uint16_t opcode ) // 4xkk - SNE Vx, byte : Skip next instruction if Vx != kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t immediate = opcode & 0xFF;

	if( get_register( reg_x ) != immediate )
		set_PC( get_PC() + 2 );
}

void Chip8::SER( uint16_t opcode ) // 5xy0 - SE Vx, Vy : Skip next instruction if Vx = Vy.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0:
		if( get_register(reg_x) == get_register(reg_y) )
			set_PC( get_PC() + 2 );
		break;

	// 0x1 .. 0xF not defined

	default:
		break;
	}
}

void Chip8::LD( uint16_t opcode ) // 6xkk - LD Vx, byte : Set Vx = kk.
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t immediate = opcode & 0xFF;

	set_register( reg_x, immediate );
}

void Chip8::ADD( uint16_t opcode ) // 7xkk - ADD Vx, byte : Set Vx = Vx + kk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	set_register( reg_x, get_register(reg_x) + (opcode & 0xFF) );
}

void Chip8::MathOp( uint16_t opcode ) // 8xyn - Various mathematical and logical operations
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0: // LD Vx, Vy : Set Vx = Vy.
		set_register( reg_x, get_register(reg_y) );
		break;

	case 0x1: // OR Vx, Vy : Set Vx = Vx OR Vy
		set_register( reg_x, get_register(reg_x) | get_register(reg_y) );
		if( memory[Quirk_index] & QUIRK_RESET )
			set_register( 0x0F, 0 );
		break;

	case 0x2: // AND Vx, Vy : Set Vx = Vx AND Vy
		set_register( reg_x, get_register(reg_x) & get_register(reg_y) );
		if( memory[Quirk_index] & QUIRK_RESET )
			set_register( 0x0F, 0 );
		break;

	case 0x3: // XOR Vx, Vy : Set Vx = Vx XOR Vy
		set_register( reg_x, get_register(reg_x) ^ get_register(reg_y) );
		if( memory[Quirk_index] & QUIRK_RESET )
			set_register( 0x0F, 0 );
		break;

	case 0x4: // ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry
		{
			const uint16_t result = get_register(reg_x) + get_register(reg_y);
			set_register( reg_x, result & 0xFF );
			set_register( 0x0F, ( result > 255 ) ? 1 : 0 );

		}
		break;

	case 0x5: // SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
		{
			const uint16_t result = ( get_register(reg_x) >= get_register(reg_y) ) ? 1 : 0;
			set_register( reg_x, get_register(reg_x) - get_register(reg_y) );
			set_register( 0x0F, result );

		}
		break;

	case 0x6: // SHR Vx {, Vy} : Set Vx = Vx SHR 1 or Vx = Vy SHR 1
		{
			const uint16_t result = ( get_register(reg_x) & 0x01 );

			if( memory[Quirk_index] & QUIRK_SHIFTING )
				set_register( reg_x, get_register(reg_x) >> 1);
			else
				set_register( reg_x, get_register(reg_y) >> 1);

			set_register( 0x0F, result );
		}
		break;

	case 0x7: // SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
		{
			const uint16_t result = ( get_register(reg_y) > get_register(reg_x) ) ? 1 : 0;
			set_register( reg_x, get_register(reg_y) - get_register(reg_x) );
			set_register( 0x0F, result );
		}
		break;

	// opcodes 0x8 .. 0xD not defined

	case 0xE: // SHL Vx {, Vy} : Set Vx = Vx SHL 1 or Vx = Vy SHL 1
		{
			const uint16_t result = ( get_register(reg_x) & 0x80 ) ? 1 : 0;

			if( memory[Quirk_index] & QUIRK_SHIFTING )
				set_register( reg_x, get_register(reg_x) << 1 );
			else
				set_register( reg_x, get_register(reg_y) << 1 );

			set_register( 0x0F, result );
		}
		break;

	// opcodes 0xF not defined

	default:
		break;
	}
}

void Chip8::SNE( uint16_t opcode ) // 9xy0 - SNE Vx, Vy : Skip next instruction if Vx != Vy
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0:
		if( get_register(reg_x) != get_register(reg_y) )
			set_PC( get_PC() + 2 );
		break;

	// opcodes 0x1 to 0xF not defined
	default: break;
	}
}

void Chip8::LDI( uint16_t opcode ) // Annn - LD I, addr : Set I = nnn
{
	const u_int16_t address = opcode & 0xFFF;

	set_word( I_index, address );
}

void Chip8::JMP( uint16_t opcode )	// Bnnn - JP V0, addr : Jump to location nnn + V0
									// or Bxnn : Jump to location nn + V[x]
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	if( memory[Quirk_index] & QUIRK_JUMPING )
		set_PC( ( opcode & 0xFF ) + get_register( reg_x ) );	// ???
	else
		set_PC( ( opcode & 0xFFF ) + get_register(0) );
}

void Chip8::RND( uint16_t opcode ) // Cxkk - RND Vx, byte : Set Vx = random byte AND kk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t immediate = ( opcode & 0xFF );

	set_register( reg_x, get_random_value() & immediate );
}

void Chip8::DRW( uint16_t opcode ) // Dxyn - DRW Vx, Vy, nibble : Display n-byte sprite starting at memory location I at
								   // (Vx, Vy), set VF = collision
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	const uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	set_register( 0x0F, 0 );
	uint8_t ypos = get_register( reg_y ) % 32;

	if( (memory[Quirk_index] & QUIRK_DISP_WAIT ) && !memory[int_index] ) { // rate limit the DRW calls to 60fps
		set_PC( get_PC() - 2 );
		return;
	}

	uint8_t collision = get_register( 0x0F );
	const uint8_t end_row = opcode & 0xF;
	for( uint8_t row = 0; row < end_row; ++row ) {

		const uint8_t sprite_byte = memory[ get_I() + row ];
		uint8_t xpos = get_register(reg_x) % 64;

		for( uint8_t bit_offset = 0; bit_offset < 8; ++bit_offset ) {
			if( sprite_byte & ( 1 << ( 7 - bit_offset ) ) )
				if( toggle_a_pixel( xpos % 64, ypos % 32 ) )
					collision |= 0x01;

			++xpos;

			if( (memory[Quirk_index] & QUIRK_CLIPPING ) && ( xpos == 64 ) )
				break;
		}

		++ypos;

		if( (memory[Quirk_index] & QUIRK_CLIPPING ) && ( ypos == 32 ) )
			break;
	}

	set_register( 0x0F, collision );
}

void Chip8::Key( uint16_t opcode ) // 0xExkk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	switch( opcode & 0xFF ) {

	// opcodes 0x00 .. 0x9D not defined

	case 0x9E: // Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed
		if( is_key_pressed( get_register(reg_x) ) )
			set_PC( get_PC() + 2 );
		break;

	// opcodes 0x9F .. 0xA0 not defined

	case 0xA1: // ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
		if( !is_key_pressed( get_register(reg_x) ) )
			set_PC( get_PC() + 2 );
		break;

	// opcodes 0xA2 .. 0xFF not defined

	default:
		break;
	}
}

void Chip8::Misc( uint16_t opcode ) // 0xFxkk
{
	const uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	switch( opcode & 0xFF ) {

	// opcodes 0x00 .. 0x06 not defined

	case 0x07: // Fx07 - LD Vx, DT : Set Vx = delay timer value.
		set_register( reg_x, get_delay_timer() );
		break;

	// opcodes 0x08 .. 0x09 not defined

	case 0x0A: // Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
		{
			uint8_t key_no = 0;
			if( key_captured( key_no ) )
				set_register( reg_x, key_no );
			else
				set_PC( get_PC() - 2 );
		}
		break;

	// opcodes 0x0B .. 0x1 not defined

	case 0x15: // Fx15 - LD DT, Vx : Set delay timer = Vx.
		set_delay_timer( get_register(reg_x) );
		break;

	// opcodes 0x16 and 0x17 not defined

	case 0x18: // Fx18 - LD ST, Vx : Set sound timer = Vx.
		set_sound_timer( get_register(reg_x) );
		break;

	// opcodes 0x19 .. 0x1D not defined

	case 0x1E: // Fx1E - ADD I, Vx : Set I = I + Vx
		set_I( get_I() + get_register(reg_x) );
		break;

	// opcodes 0x1F .. 0x28 not defined

	case 0x29: // Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
		set_I( font_sprite_base + 5 * get_register(reg_x) );
		break;

	// opcodes 0x2A .. 0x32 not defined

	case 0x33: // Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2
		memory[get_I()] = get_register(reg_x) / 100;
		memory[get_I() + 1] = ( get_register(reg_x) / 10 ) % 10;
		memory[get_I() + 2] = get_register(reg_x) % 10;
		break;

	// opcodes 0x34 .. 0x54 not defined

	case 0x55: // Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I
		{
			int idx = 0;
			uint16_t I_base = get_I();

			for( ; idx <= reg_x; ++idx )
				memory[I_base + idx] = get_register(idx);

			if( memory[Quirk_index] & QUIRK_MEMORY )
				set_I( get_I() + idx );

		}
		break;

	// opcodes 0x56 .. 0x64 not defined

	case 0x65: // Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I
		{
			int idx = 0;
			uint16_t I_base = get_I();

			for( ; idx <= reg_x; ++idx )
				set_register( idx,  memory[I_base + idx] );

			if( memory[Quirk_index] & QUIRK_MEMORY )
				set_I( get_I() + idx );

		}
		break;

	// opcodes 0x66 .. 0xFF not defined

	default:
		break;
	}
}
