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
#include "utils.h"

DecodeResult linear( uint16_t address, uint16_t opcode, std::string mnemonic, std::string argument = "" )
{
	uint16_t next_address = address + 2;

    return {
        Instruction{ address, opcode, mnemonic, argument, 0 },
        AddressList{ next_address },
        std::nullopt
    };
}

DecodeResult skip( uint16_t address, uint16_t opcode, std::string mnemonic, std::string argument )
{
	uint16_t next_address = address + 2;
	uint16_t skip_address = address + 4;

    return {
        Instruction{ address, opcode, mnemonic, argument, 0 },
        AddressList{ next_address, skip_address },
        std::nullopt
    };
}

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

DecodeResult Decoder::decode( uint16_t address, Memory &memory )
{
	uint16_t opcode = memory.get_word( address );

	return (this->*dispatcher[(opcode >> 12) & 0x0F])( address, opcode );
}

DecodeResult Decoder::decode_SYS( uint16_t address, uint16_t opcode )
{
	switch( opcode & 0xFFF ) {

	/* opcodes 0000 .. 00DF not defined */

	case 0x0E0: // CLS : clear screen
		return linear( address, opcode, "CLS" );

	/* opcodes 00E1 .. 00ED not defined */

	case 0x0EE: // RET : return from subroutine
		return {
			Instruction{ address, opcode, "RET", "", 0 },
			AddressList{},
			std::nullopt
		};

	/* opcodes 00EF .. 0FFF not defined */

	default:
		return linear( address, opcode, "NOP" );
	}
}

DecodeResult Decoder::decode_LD( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t byte = ( opcode & 0xFF );

	return linear( address, opcode, "LD", format_register( reg_x ) + ", " + format_byte( byte ) );
}

DecodeResult Decoder::decode_ADD( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t byte = ( opcode & 0xFF );

	return linear( address, opcode, "ADD", format_register( reg_x ) + ", " + format_byte( byte ) );
}

DecodeResult Decoder::decode_MathOp( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0: // LD Vx, Vy : Set Vx = Vy.
		return linear( address, opcode, "LD", format_register( reg_x ) + ", " + format_register( reg_y ) );

	case 0x1: // OR Vx, Vy : Set Vx = Vx OR Vy
		return linear( address, opcode, "OR", format_register( reg_x ) + ", " + format_register( reg_y ) );

	case 0x2: // AND Vx, Vy : Set Vx = Vx AND Vy
		return linear( address, opcode, "AND", format_register( reg_x ) + ", " + format_register( reg_y ) );

	case 0x3: // XOR Vx, Vy : Set Vx = Vx XOR Vy
		return linear( address, opcode, "XOR", format_register( reg_x ) + ", " + format_register( reg_y ) );

	case 0x4: // ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry
		return linear( address, opcode, "ADD", format_register( reg_x ) + ", " + format_register( reg_y ) );

	case 0x5: // SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
		return linear( address, opcode, "SUB", format_register( reg_x ) + ", " + format_register( reg_y ) );

	/*
	 * Note from Wikipedia:
	 *
	 * 	CHIP-8's opcodes 8XY6 and 8XYE (the bit shift instructions), which were in fact undocumented opcodes in the
	 *original interpreter, shifted the value in the register VY and stored the result in VX. The CHIP-48 and SCHIP
	 *implementations instead ignored VY, and simply shifted VX
	 */
	case 0x6: // SHR Vx {, Vy} : Set Vx = Vx SHR 1.	Error in documentation! should be Vx = Vy SHR 1
		return linear( address, opcode, "SHR", format_register( reg_x ) + " {, " + format_register( reg_y ) + "}" );

	case 0x7: // SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
		return linear( address, opcode, "SUBN", format_register( reg_x ) + ", " + format_register( reg_y ) );

	/* opcodes 8xy8 .. 8xyD not defined */
	case 0xE: // SHL Vx {, Vy} : Set Vx = Vx SHL 1. Error in documentation, should be Vx = Vy SHL 1
		return linear( address, opcode, "SHL", format_register( reg_x ) + " {, " + format_register( reg_y ) + "}" );

	/* opcode 8xyF not defined */
	default:
		return linear( address, opcode, "NOP" );
	}
}

DecodeResult Decoder::decode_RND( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t byte = ( opcode >> 0 ) & 0xFF;

	return linear( address, opcode, "RND", format_register( reg_x ) + ", " + format_byte( byte ) );
}

DecodeResult Decoder::decode_DRW( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t reg_y = ( opcode >> 4 ) & 0xF;
	unsigned int nibble = ( opcode >> 0 ) & 0xF;

	return linear( address, opcode, "DRW", format_register( reg_x ) + ", " + format_register( reg_y ) + ", " + std::to_string( nibble ) );
}

DecodeResult Decoder::decode_Misc( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	switch( opcode & 0xFF ) {
	/* opcodes Fx00 .. Fx06 not defined */
	case 0x07: // Fx07 - LD Vx, DT : Set Vx = delay timer value.
		return linear( address, opcode, "LD", format_register( reg_x ) + ", DT" );

	/* opcodes Fx08 .. Fx09 not defined */
	case 0x0A: // Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
		return linear( address, opcode, "LD", format_register( reg_x ) + ", K" );

	/* opcodes Fx0B .. Fx14 not defined */
	case 0x15: // Fx15 - LD DT, Vx : Set delay timer = Vx.
		return linear( address, opcode, "LD", "DT, " + format_register( reg_x ) );

	/* opcodes Fx16 and Fx17 not defined */
	case 0x18: // Fx18 - LD ST, Vx : Set sound timer = Vx.
		return linear( address, opcode, "LD", "ST, " + format_register( reg_x ) );

	/* opcodes Fx19 .. Fx1D not defined */
	case 0x1E: // Fx1E - ADD I, Vx : Set I = I + Vx
		return linear( address, opcode, "ADD", "I, " + format_register( reg_x ) );

	/* opcodes Fx1F .. Fx28 not defined */
	case 0x29: // Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
		return linear( address, opcode, "LD", "I, &CHR[" + format_register( reg_x ) + "]" );

	/* opcodes Fx2A .. Fx32 not defined */
	case 0x33: // Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2
		return linear( address, opcode, "LD", "[I], BCD[" + format_register( reg_x ) + "]" );

	/* opcodes Fx34 .. Fx54 not defined */
	case 0x55: // Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I
		return linear( address, opcode, "LD", "[I], V0 - " + format_register( reg_x ) );

	/* opcodes Fx56 .. Fx64 not defined */
	case 0x65: // Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I
		return linear( address, opcode, "LD", "V0 - " + format_register( reg_x ) + ", [I]" );

	/* opcodes Fx66 .. FxFF not defined */
	default:
		return linear( address, opcode, "NOP" );
	}
}

DecodeResult Decoder::decode_SEI( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t byte = ( opcode & 0xFF );

	return skip( address, opcode, "SE", format_register( reg_x ) + ", " + format_byte( byte ) );
}

DecodeResult Decoder::decode_SNEI( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t byte = ( opcode & 0xFF );

	return skip( address, opcode, "SNE", format_register( reg_x ) + ", " + format_byte( byte ) );
}

DecodeResult Decoder::decode_SER( uint16_t address, uint16_t opcode )
{
	switch( opcode & 0xF ) {
	case 0x0: {
			uint8_t reg_x = ( opcode >> 8 ) & 0xF;
			uint8_t reg_y = ( opcode >> 4 ) & 0xF;

			return skip( address, opcode, "SE", format_register( reg_x ) + ", " + format_register( reg_y ) );
		}
		break;

	/* opcodes 5xy1 .. 5xyF not defined */

	default:
		return linear( address, opcode, "NOP" );
	}
}

DecodeResult Decoder::decode_SNE( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;
	uint8_t reg_y = ( opcode >> 4 ) & 0xF;

	return skip( address, opcode, "SNE", format_register( reg_x ) + ", " + format_register( reg_y ) );
}

DecodeResult Decoder::decode_Key( uint16_t address, uint16_t opcode )
{
	uint8_t reg_x = ( opcode >> 8 ) & 0xF;

	switch( opcode & 0xFF ) {
	/* opcodes Ex00 .. Ex9D not defined */
	case 0x9E: // Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed
		return skip( address, opcode, "SKP", format_register( reg_x ) );

	/* opcodes Ex9F .. ExA0 not defined */
	case 0xA1: // ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
		return skip( address, opcode, "SKNP", format_register( reg_x ) );

	/* opcodes ExA2 .. ExFF not defined */
	default:
		return linear( address, opcode, "NOP" );
	}
}

DecodeResult Decoder::decode_JP( uint16_t address, uint16_t opcode )
{
	uint16_t jmp_address = opcode & 0xFFF;

	return DecodeResult {
		Instruction{ address, opcode, "JP", "", jmp_address },
		AddressList { jmp_address },
		DecodedTarget { jmp_address, eKind::JUMP }
	};
}

DecodeResult Decoder::decode_CALL( uint16_t address, uint16_t opcode )
{
	uint16_t jmp_address = opcode & 0xFFF;
	uint16_t next_address = address + 2;

	return DecodeResult {
		Instruction { address, opcode, "CALL", "", jmp_address },
		AddressList { next_address, jmp_address },
		DecodedTarget { jmp_address, eKind::SUBROUTINE }
	};
}

DecodeResult Decoder::decode_LDI( uint16_t address, uint16_t opcode )
{
	uint16_t load_address = opcode & 0xFFF;
	uint16_t next_address = address + 2;

	return DecodeResult {
		Instruction{ address, opcode, "LD", "I, ", load_address },
		AddressList { next_address },
		DecodedTarget { load_address, eKind::I_TARGET }
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
		Instruction{ address, opcode, "JMP", "V0, ", table_base },
		AddressList { jmp_address },
		DecodedTarget { table_base, eKind::INDEXED }
	};
}
