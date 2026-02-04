/*
 * disasmmemory_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "disassembler.h"

class DisasmMemoryTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}

	DisasmMemory memory;
};

TEST_F(DisasmMemoryTest, TestBoundaries)
{
	BinImage image = {
		0x01, 0x02, 0x03, 0x04
	};
	EXPECT_EQ( memory.start(), memory.end() );
	memory.bind( image );
	EXPECT_EQ( memory.start() + 4, memory.end() );
}

TEST_F(DisasmMemoryTest, TestLimits)
{
	BinImage image = {
		0x01, 0x02, 0x03, 0x04
	};
	memory.bind( image );
	EXPECT_FALSE( memory.contains( 0x1FF ) );
	EXPECT_TRUE( memory.contains( 0x200 ) );
	EXPECT_TRUE( memory.contains( 0x203 ) );
	EXPECT_FALSE( memory.contains( 0x204 ) );
}

TEST_F(DisasmMemoryTest, GetBytes)
{
	BinImage image = {
		0x01, 0x02, 0x03, 0x04
	};
	memory.bind( image );
	EXPECT_EQ( memory.get_byte( 0x200 ), 0x01 );
	EXPECT_EQ( memory.get_byte( 0x201 ), 0x02 );
	EXPECT_EQ( memory.get_byte( 0x202 ), 0x03 );
	EXPECT_EQ( memory.get_byte( 0x203 ), 0x04 );
}

TEST_F(DisasmMemoryTest, GetWords)
{
	BinImage image = {
		0x01, 0x02, 0x03, 0x04
	};
	memory.bind( image );
	EXPECT_EQ( memory.get_word( 0x200 ), 0x0102 );
	EXPECT_EQ( memory.get_word( 0x201 ), 0x0203 );
	EXPECT_EQ( memory.get_word( 0x202 ), 0x0304 );
}

TEST_F(DisasmMemoryTest, CheckValidInstructionMark)
{
	BinImage image = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06
	};
	memory.bind( image );

	memory.mark_instruction( 0x202 );

	EXPECT_FALSE( memory.is_instruction( 0x200) );
	EXPECT_TRUE( memory.is_instruction( 0x202) );
	EXPECT_FALSE( memory.is_instruction( 0x204) );
}

TEST_F(DisasmMemoryTest, CheckUnalignedInstructionMark)
{
	BinImage image = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06
	};
	memory.bind( image );

	memory.mark_instruction( 0x203 );

	EXPECT_FALSE( memory.is_instruction( 0x200) );
	EXPECT_FALSE( memory.is_instruction( 0x202) );
	EXPECT_TRUE( memory.is_instruction( 0x203) );
	EXPECT_FALSE( memory.is_instruction( 0x205) );
}
