/*
 * binary_encoder.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <array>
#include <cassert>

#include "ir/encoder.h"
#include "encoder.h"

static const std::array<uint16_t, opcode_count> base =
{
	/* OPCODE::NOP */		0x0000,
	/* Opcode::CLS */		0x00E0,
	/* Opcode::RET */		0x00EE,
	/* Opcode::JP */		0x1000,
	/* Opcode::CALL */		0x2000,
	/* Opcode::SE_Imm */	0x3000,
	/* Opcode::SNE_Imm */	0x4000,
	/* Opcode::SE_Reg */	0x5000,
	/* Opcode::LD_Imm */	0x6000,
	/* Opcode::ADD_Imm */	0x7000,
	/* Opcode::LD_Reg */	0x8000,
	/* Opcode::OR */		0x8001,
	/* Opcode::AND */		0x8002,
	/* Opcode::XOR */		0x8003,
	/* Opcode::ADD_Reg */	0x8004,
	/* Opcode::SUB */		0x8005,
	/* Opcode::SHR */		0x8006,
	/* Opcode::SUBN */		0x8007,
	/* Opcode::SHL */		0x800E,
	/* Opcode::SNE_Reg */	0x9000,
	/* Opcode::LD_I */		0xA000,
	/* Opcode::JP_V0 */		0xB000,
	/* Opcode::RND */		0xC000,
	/* Opcode::DRW */		0xD000,
	/* Opcode::SKP */		0xE09E,
	/* Opcode::SKNP */		0xE0A1,
	/* Opcode::LD_DT */		0xF015,
	/* Opcode::LD_ST */		0xF018,
	/* Opcode::ST_KEY */	0xF00A,
	/* Opcode::ST_DT */		0xF007,
	/* Opcode::ADD_I */		0xF01E,
	/* Opcode::LD_SPRITE */	0xF029,
	/* Opcode::BCD */		0xF033,
	/* Opcode::ST_REGS */	0xF055,
	/* Opcode::LD_REGS */	0xF065,
};

BinImage BinaryEncoder::encode( const IRProgram &ir )
{
	BinImage image;

	for( const auto& element : ir.elements )
        std::visit( [&]( const auto& v ) { encode_element( v, image ); }, element );

	return image;
}

void BinaryEncoder::encode_element( const InstructionElement& element, BinImage& image )
{
	const auto& opcode = element.instruction.opcode();
	const auto& operands = element.instruction.operands();

	// uint16_t result = encoding_table.at(opcode).base;
	uint16_t result = base[ static_cast<size_t>(opcode) ];

	switch( opcode ) {
	// no operand
	case Opcode::CLS:
	case Opcode::RET:
		assert( operands.size() == 0 );
		break;

	// 1 operand; address
	case Opcode::JP:
	case Opcode::CALL:
	case Opcode::LD_I:
	case Opcode::JP_V0:
		assert( operands.size() == 1 );
		result |= (std::get<Addr>(operands[0]).value);
		break;

	// 1 operand; x register
	case Opcode::LD_DT:
	case Opcode::LD_ST:
	case Opcode::ST_KEY:
	case Opcode::ST_DT:
	case Opcode::ADD_I:
	case Opcode::LD_SPRITE:
	case Opcode::BCD:
		assert( operands.size() == 1 );
		result |= (std::get<Reg>(operands[0]).index << 8);
		break;

	// 1 operand; key index
	case Opcode::SKP:
	case Opcode::SKNP:
		assert( operands.size() == 1 );
		result |= (std::get<Key>(operands[0]).index << 8);
		break;

	// 1 operand; register count
	case Opcode::ST_REGS:
	case Opcode::LD_REGS:
		assert( operands.size() == 1 );
		result |= (std::get<RegCount>(operands[0]).count << 8);
		break;

	// 2 operands; x register and immediate
	case Opcode::SE_Imm:
	case Opcode::SNE_Imm:
	case Opcode::LD_Imm:
	case Opcode::ADD_Imm:
	case Opcode::RND:
		assert( operands.size() == 2 );
		result |= (std::get<Reg>(operands[0]).index << 8);
		result |= (std::get<Imm>(operands[1]).value);
		break;

	// 2 operands; x register and y register
	case Opcode::SE_Reg:
	case Opcode::LD_Reg:
	case Opcode::OR:
	case Opcode::AND:
	case Opcode::XOR:
	case Opcode::ADD_Reg:
	case Opcode::SUB:
	case Opcode::SHR:
	case Opcode::SUBN:
	case Opcode::SHL:
	case Opcode::SNE_Reg:
		assert( operands.size() == 2 );
		result |= (std::get<Reg>(operands[0]).index << 8);
		result |= (std::get<Reg>(operands[1]).index << 4);
		break;

	// 3 operands; x register, y register and nibble
	case Opcode::DRW:
		assert( operands.size() == 3 );
		result |= (std::get<Reg>(operands[0]).index << 8);
		result |= (std::get<Reg>(operands[1]).index << 4);
		result |= (std::get<Nibble>(operands[2]).value );
		break;

	default:
		assert( false && "Unhandled opcode in encoder" );
	}

	image.push_back( (result >> 8) & 0xFF );
	image.push_back( (result     ) & 0xFF );
	return;
}

void BinaryEncoder::encode_element( const DataElement& element, BinImage& image )
{
	image.insert( image.end(), element.byte_run.begin(), element.byte_run.end() );
}
