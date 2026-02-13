/*
 * decoder.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "decoder.h"

#include "memory.h"

Decoder::Dispatcher Decoder::dispatcher =
{
	 &Decoder::decode_SYS,
	 &Decoder::decode_JP,
	 &Decoder::decode_CALL,
	 &Decoder::decode_SEI,
	 &Decoder::decode_SNEI,
	 &Decoder::decode_SER,
	 &Decoder::decode_LD,
	 &Decoder::decode_ADD,
	 &Decoder::decode_MathOp,
	 &Decoder::decode_SNE,
	 &Decoder::decode_LDI,
	 &Decoder::decode_JMP,
	 &Decoder::decode_RND,
	 &Decoder::decode_DRW,
	 &Decoder::decode_Key,
	 &Decoder::decode_Misc,
};

DecodeResult Decoder::decode( uint16_t address, uint16_t opcode )
{
	return (this->*dispatcher[(opcode >> 12) & 0x0F])( address, opcode );
}

DecodeResult Decoder::decode_SYS( uint16_t address, uint16_t opcode )
{
	uint16_t next_address = address + 2;

	switch( opcode & 0xFFF ) {

	/* opcodes 0000 .. 00DF not defined */

	case 0x0E0: // CLS : clear screen
		return { Instruction::make_clear(), AddressList{ next_address }, std::nullopt };

	/* opcodes 00E1 .. 00ED not defined */

	case 0x0EE: // RET : return from subroutine
		return { Instruction::make_return(), AddressList{}, std::nullopt };

	/* opcodes 00EF .. 0FFF not defined */

	default:
		return { Instruction::make_nop(), AddressList{ next_address }, std::nullopt, false };
	}
}

DecodeResult Decoder::decode_LD( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Imm byte { uint8_t(opcode & 0xFF) };
	uint16_t next_address( address + 2 );

	return { Instruction::make_ld( reg_x, byte ), AddressList{ next_address }, std::nullopt };
}

DecodeResult Decoder::decode_ADD( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Imm byte { uint8_t(opcode & 0xFF) };
	uint16_t next_address( address + 2 );

	return { Instruction::make_add( reg_x, byte ), AddressList{ next_address }, std::nullopt };
}

DecodeResult Decoder::decode_MathOp( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Reg reg_y { uint8_t((opcode >> 4) & 0xF) };
	uint16_t next_address( address + 2 );

	switch( opcode & 0xF ) {
	case 0x0: // LD Vx, Vy : Set Vx = Vy.
		return { Instruction::make_ld( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	case 0x1: // OR Vx, Vy : Set Vx = Vx OR Vy
		return { Instruction::make_or( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	case 0x2: // AND Vx, Vy : Set Vx = Vx AND Vy
		return { Instruction::make_and( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	case 0x3: // XOR Vx, Vy : Set Vx = Vx XOR Vy
		return { Instruction::make_xor( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	case 0x4: // ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry
		return { Instruction::make_add( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	case 0x5: // SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
		return { Instruction::make_sub( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	/*
	 * Note from Wikipedia:
	 *
	 * 	CHIP-8's opcodes 8XY6 and 8XYE (the bit shift instructions), which were in fact undocumented opcodes in the
	 *original interpreter, shifted the value in the register VY and stored the result in VX. The CHIP-48 and SCHIP
	 *implementations instead ignored VY, and simply shifted VX
	 */
	case 0x6: // SHR Vx {, Vy} : Set Vx = Vx SHR 1.	Error in documentation! should be Vx = Vy SHR 1
		return { Instruction::make_shift_right( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	case 0x7: // SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
		return { Instruction::make_subn( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	/* opcodes 8xy8 .. 8xyD not defined */
	case 0xE: // SHL Vx {, Vy} : Set Vx = Vx SHL 1. Error in documentation, should be Vx = Vy SHL 1
		return { Instruction::make_shift_left( reg_x, reg_y ), AddressList{ next_address }, std::nullopt };

	/* opcode 8xyF not defined */
	default:
		return { Instruction::make_nop(), AddressList{ next_address }, std::nullopt, false };
	}
}

DecodeResult Decoder::decode_RND( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Imm byte { uint8_t(opcode & 0xFF) };
	uint16_t next_address( address + 2 );

	return { Instruction::make_rnd( reg_x, byte ), AddressList{ next_address }, std::nullopt };
}

DecodeResult Decoder::decode_DRW( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Reg reg_y { uint8_t((opcode >> 4) & 0xF) };
	Nibble nibble { uint8_t(opcode & 0xF) };
	uint16_t next_address( address + 2 );

	return { Instruction::make_drw( reg_x, reg_y, nibble ), AddressList{ next_address }, std::nullopt };
}

DecodeResult Decoder::decode_Misc( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	RegCount rc { uint8_t((opcode >> 8) & 0xF) };
	uint16_t next_address( address + 2 );

	switch( opcode & 0xFF ) {
	/* opcodes Fx00 .. Fx06 not defined */
	case 0x07: // Fx07 - LD Vx, DT : Set Vx = delay timer value.
		return { Instruction::make_store_delay_timer( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx08 .. Fx09 not defined */
	case 0x0A: // Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
		return { Instruction::make_store_key( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx0B .. Fx14 not defined */
	case 0x15: // Fx15 - LD DT, Vx : Set delay timer = Vx.
		return { Instruction::make_load_delay_timer( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx16 and Fx17 not defined */
	case 0x18: // Fx18 - LD ST, Vx : Set sound timer = Vx.
		return { Instruction::make_load_sound_timer( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx19 .. Fx1D not defined */
	case 0x1E: // Fx1E - ADD I, Vx : Set I = I + Vx
		return { Instruction::make_add_i( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx1F .. Fx28 not defined */
	case 0x29: // Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
		return { Instruction::make_sprite_for( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx2A .. Fx32 not defined */
	case 0x33: // Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2
		return { Instruction::make_bcd( reg_x ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx34 .. Fx54 not defined */
	case 0x55: // Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I
		return { Instruction::make_save_regs( rc ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx56 .. Fx64 not defined */
	case 0x65: // Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I
		return { Instruction::make_load_regs( rc ), AddressList{ next_address }, std::nullopt };

	/* opcodes Fx66 .. FxFF not defined */
	default:
		return { Instruction::make_nop(), AddressList{ next_address }, std::nullopt, false };
	}
}

DecodeResult Decoder::decode_SEI( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Imm byte { uint8_t(opcode & 0xFF) };
	uint16_t next_address = address + 2;
	uint16_t skip_address = address + 4;

    return { Instruction::make_skip_eq( reg_x, byte ), AddressList{ next_address, skip_address }, std::nullopt };
}

DecodeResult Decoder::decode_SNEI( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Imm byte { uint8_t(opcode & 0xFF) };
	uint16_t next_address = address + 2;
	uint16_t skip_address = address + 4;

    return { Instruction::make_skip_neq( reg_x, byte ), AddressList{ next_address, skip_address }, std::nullopt };
}

DecodeResult Decoder::decode_SER( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Reg reg_y { uint8_t((opcode >> 4) & 0xF) };
	uint16_t next_address = address + 2;
	uint16_t skip_address = address + 4;

	switch( opcode & 0xF ) {
	case 0x0:
   		return { Instruction::make_skip_eq( reg_x, reg_y ), AddressList{ next_address, skip_address }, std::nullopt };

	/* opcodes 5xy1 .. 5xyF not defined */

	default:
		return { Instruction::make_nop(), AddressList{ next_address }, std::nullopt, false };
	}
}

DecodeResult Decoder::decode_SNE( uint16_t address, uint16_t opcode )
{
	Reg reg_x { uint8_t((opcode >> 8) & 0xF) };
	Reg reg_y { uint8_t((opcode >> 4) & 0xF) };
	uint16_t next_address = address + 2;
	uint16_t skip_address = address + 4;

	if( opcode & 0x0F )
		return { Instruction::make_nop(), AddressList{ next_address }, std::nullopt, false };

    return { Instruction::make_skip_neq( reg_x, reg_y ), AddressList{ next_address, skip_address }, std::nullopt };
}

DecodeResult Decoder::decode_Key( uint16_t address, uint16_t opcode )
{
	Key key { uint8_t((opcode >> 8) & 0xF) };
	uint16_t next_address = address + 2;
	uint16_t skip_address = address + 4;

	switch( opcode & 0xFF ) {
	/* opcodes Ex00 .. Ex9D not defined */
	case 0x9E: // Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed
    	return { Instruction::make_skip_if_key( key ), AddressList{ next_address, skip_address }, std::nullopt };

	/* opcodes Ex9F .. ExA0 not defined */
	case 0xA1: // ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
    	return { Instruction::make_skip_not_key( key ), AddressList{ next_address, skip_address }, std::nullopt };

	/* opcodes ExA2 .. ExFF not defined */
	default:
		return { Instruction::make_nop(), AddressList{ next_address }, std::nullopt, false };
	}
}

DecodeResult Decoder::decode_JP( uint16_t address, uint16_t opcode )
{
	uint16_t jmp_address = opcode & 0xFFF;

	return DecodeResult {
		Instruction::make_jump( Addr { jmp_address } ),
		AddressList { jmp_address },
		DecodedSymbol { jmp_address, eSymbolKind::JUMP }
	};
}

DecodeResult Decoder::decode_CALL( uint16_t address, uint16_t opcode )
{
	uint16_t jmp_address = opcode & 0xFFF;
	uint16_t next_address = address + 2;

	return DecodeResult {
		Instruction::make_call( Addr { jmp_address } ),
		AddressList { next_address, jmp_address },
		DecodedSymbol { jmp_address, eSymbolKind::SUBROUTINE }
	};
}

DecodeResult Decoder::decode_LDI( uint16_t address, uint16_t opcode )
{
	uint16_t load_address = opcode & 0xFFF;
	uint16_t next_address = address + 2;

	return DecodeResult {
		Instruction::make_ld_i( Addr { load_address } ),
		AddressList { next_address },
		DecodedSymbol { load_address, eSymbolKind::I_TARGET }
	};
}

DecodeResult Decoder::decode_JMP( uint16_t address, uint16_t opcode )
{
	uint16_t table_base = ( opcode >> 0 ) & 0xFFF;
	/*
		This looks a problem; we do not know the value of V0, so we cannot mark
		the proper address as an instruction. However this is going to be resovled
		at a higher level of abstraction, so putting table base in the AddressList
		is the right thing to do here.
	*/
	uint16_t jmp_address = table_base /* + [V0]*/;

	return DecodeResult {
		Instruction::make_jump_indexed( Addr { jmp_address } ),
		AddressList { jmp_address },
		DecodedSymbol { table_base, eSymbolKind::INDEXED }
	};
}
