/*
 * integration_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <gtest/gtest.h>

#include "ir/chip8formats.h"
#include "ir/chip8ir.h"

#include "ir/encoder.h"
#include "ir/decoder.h"

#include "disassembler/disassembler.h"

class IntegrationTest : public ::testing::Test
{
protected:
	BinaryEncoder encoder;
    IRProgram ir;
	Decoder decoder;

	void SetUp() override {

	}
	void TearDown() override {
	}

	static uint16_t make_word(const BinImage& img, size_t offset = 0)
	{
		return (static_cast<uint16_t>(img[offset]) << 8) |  static_cast<uint16_t>(img[offset + 1]);
	}
};

TEST_F(IntegrationTest, EncoderDecoderRoundtrip)
{
	std::vector<Instruction> cases = {
		Instruction::make_clear(),
		Instruction::make_return(),
		Instruction::make_jump(Addr{0x123}),
		Instruction::make_ld(Reg{2}, Imm{0xAA}),
		Instruction::make_add(Reg{2}, Reg{7}),
		Instruction::make_drw(Reg{1}, Reg{2}, Nibble{5}),
		Instruction::make_skip_if_key(Key{4}),
		Instruction::make_save_regs(RegCount{8}),
	};

	for (const auto& instr : cases)
	{
		IRProgram ir;
		ir.elements.push_back( InstructionElement{ 0x200, instr });

		auto image = encoder.encode(ir);

		Disassembler dis;

		dis.configure( {0x200} );

		auto reconstructed = dis.build_ir( image );

		EXPECT_EQ( reconstructed.ir, ir );
	}
}

TEST_F(IntegrationTest, EncoderDecoderRoundtripMultiStatement)
{
	IRProgram ir;

	ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_clear() } );
	ir.elements.push_back( InstructionElement{ 0x202, Instruction::make_jump(Addr{0x208}) } );
	ir.elements.push_back( DataElement{ 0x204, { 0x01, 0x02, 0x03, 0x04 } } );
	ir.elements.push_back( InstructionElement{ 0x208, Instruction::make_ld(Reg{2}, Imm{0xAA}) } );
	ir.elements.push_back( InstructionElement{ 0x20A, Instruction::make_add(Reg{2}, Reg{7}) });
	ir.elements.push_back( InstructionElement{ 0x20C, Instruction::make_drw(Reg{1}, Reg{2}, Nibble{5})});
	ir.elements.push_back( InstructionElement{ 0x20E, Instruction::make_skip_if_key(Key{4})});
	ir.elements.push_back( InstructionElement{ 0x210, Instruction::make_save_regs(RegCount{8})});

	auto image = encoder.encode(ir);

	Disassembler dis;
	dis.configure( {0x200} );

	auto reconstructed = dis.build_ir( image );

	EXPECT_EQ( reconstructed.ir, ir );
}