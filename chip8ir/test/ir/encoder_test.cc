/*
 * encoder_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

class EncoderTest : public ::testing::Test
{
protected:
	BinaryEncoder encoder;
    IRProgram ir;

	void SetUp() override {}
	void TearDown() override {}

	static uint16_t make_word(const BinImage& img, size_t offset = 0)
	{
		return (static_cast<uint16_t>(img[offset]) << 8) |  static_cast<uint16_t>(img[offset + 1]);
	}
};

TEST_F( EncoderTest, EncodesCLS )
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_clear() } );

    auto image = encoder.encode(ir);

    ASSERT_EQ( image.size(), 2 );
    EXPECT_EQ( make_word(image), 0x00E0 );
}

TEST_F(EncoderTest, Encodes_JP_Address)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_jump( Addr { 0x234 }) } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x1234);
}

TEST_F(EncoderTest, Encodes_LD_Imm)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_ld( Reg{1}, Imm{0xAB} ) } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x61AB);
}

TEST_F(EncoderTest, Encodes_ADD_Reg)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_add( Reg{2}, Reg{3} ) } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x8234);
}

TEST_F(EncoderTest, Encodes_DRW)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_drw( Reg{1}, Reg{2}, Nibble{5} ) } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0xD125);
}

TEST_F(EncoderTest, Encodes_DataElement)
{
    ir.elements.push_back( DataElement { 0x300, { 0xDE, 0xAD, 0xBE, 0xEF } } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 4);

    EXPECT_EQ(image[0], 0xDE);
    EXPECT_EQ(image[1], 0xAD);
    EXPECT_EQ(image[2], 0xBE);
    EXPECT_EQ(image[3], 0xEF);
}

TEST_F(EncoderTest, Encodes_MixedProgram)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_clear() } );
    ir.elements.push_back( InstructionElement{ 0x202, Instruction::make_ld( Reg{1}, Imm{0x10} ) } );
    ir.elements.push_back( InstructionElement{ 0x204, Instruction::make_jump( Addr { 0x300 }) } );
    ir.elements.push_back( DataElement{ 0x206, { 0xAA, 0xBB } } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 8);

    EXPECT_EQ(make_word(image, 0), 0x00E0);
    EXPECT_EQ(make_word(image, 2), 0x6110);
    EXPECT_EQ(make_word(image, 4), 0x1300);
    EXPECT_EQ(image[6], 0xAA);
    EXPECT_EQ(image[7], 0xBB);
}

TEST_F(EncoderTest, Encodes_MaxValues)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_skip_eq( Reg{15}, Imm{0xFF} ) } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x3FFF);
}

TEST_F(EncoderTest, IgnoresElementAddresses)
{
    ir.elements.push_back( InstructionElement{ 0x400, Instruction::make_clear() } );
    ir.elements.push_back( InstructionElement{ 0x100, Instruction::make_return() } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 4);
    EXPECT_EQ(make_word(image, 0), 0x00E0);
    EXPECT_EQ(make_word(image, 2), 0x00EE);
}

TEST_F(EncoderTest, Encodes_SKP)
{
    ir.elements.push_back( InstructionElement{ 0x200, Instruction::make_skip_if_key(Key{3}) } );

    auto image = encoder.encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0xE39E);
}
