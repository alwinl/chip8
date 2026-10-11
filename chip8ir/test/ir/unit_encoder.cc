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

namespace {

	uint16_t make_word(const BinImage& img, size_t offset = 0)
	{
		return (static_cast<uint16_t>(img[offset]) << 8) |  static_cast<uint16_t>(img[offset + 1]);
	}
}

TEST( UnitEncoder, EncodesCLS )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_clear() } );

    auto image = encode(ir);

    ASSERT_EQ( image.size(), 2 );
    EXPECT_EQ( make_word(image), 0x00E0 );
}

TEST( UnitEncoder, Encodes_JP_Address )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_jump( Addr { 0x234 }) } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x1234);
}

TEST( UnitEncoder, Encodes_LD_Imm )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_ld( Reg{1}, Imm{0xAB} ) } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x61AB);
}

TEST( UnitEncoder, Encodes_ADD_Reg )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_add( Reg{2}, Reg{3} ) } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x8234);
}

TEST( UnitEncoder, Encodes_DRW )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_drw( Reg{1}, Reg{2}, Nibble{5} ) } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0xD125);
}

TEST( UnitEncoder, Encodes_IRDataBlock )
{
    IRProgram ir;

    ir.elements.push_back( IRDataBlock { 0x300, { 0xDE, 0xAD, 0xBE, 0xEF } } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 4);

    EXPECT_EQ(image[0], 0xDE);
    EXPECT_EQ(image[1], 0xAD);
    EXPECT_EQ(image[2], 0xBE);
    EXPECT_EQ(image[3], 0xEF);
}

TEST( UnitEncoder, Encodes_MixedProgram )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_clear() } );
    ir.elements.push_back( IRInstruction{ 0x202, Instruction::make_ld( Reg{1}, Imm{0x10} ) } );
    ir.elements.push_back( IRInstruction{ 0x204, Instruction::make_jump( Addr { 0x300 }) } );
    ir.elements.push_back( IRDataBlock{ 0x206, { 0xAA, 0xBB } } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 8);

    EXPECT_EQ(make_word(image, 0), 0x00E0);
    EXPECT_EQ(make_word(image, 2), 0x6110);
    EXPECT_EQ(make_word(image, 4), 0x1300);
    EXPECT_EQ(image[6], 0xAA);
    EXPECT_EQ(image[7], 0xBB);
}

TEST( UnitEncoder, Encodes_MaxValues )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_skip_eq( Reg{15}, Imm{0xFF} ) } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0x3FFF);
}

TEST( UnitEncoder, IgnoresElementAddresses )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x400, Instruction::make_clear() } );
    ir.elements.push_back( IRInstruction{ 0x100, Instruction::make_return() } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 4);
    EXPECT_EQ(make_word(image, 0), 0x00E0);
    EXPECT_EQ(make_word(image, 2), 0x00EE);
}

TEST( UnitEncoder, Encodes_SKP )
{
    IRProgram ir;

    ir.elements.push_back( IRInstruction{ 0x200, Instruction::make_skip_if_key(Reg{3}) } );

    auto image = encode(ir);

    ASSERT_EQ(image.size(), 2);
    EXPECT_EQ(make_word(image), 0xE39E);
}
