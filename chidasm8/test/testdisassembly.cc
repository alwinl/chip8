/*
 * testdisassembly.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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


class ChidasmTest : public ::testing::Test {
protected:
	void SetUp() override {
		
	}
	void TearDown() override {
	}
};

TEST_F(ChidasmTest, blinky)
{
	std::vector<uint8_t> program = {
		0x00, 0xE0, // CLS
		0x01, 0x04, // SYS 104
		0x12, 0x06, // JP 206
		0x22, 0x44, // CALL 244
		0x31, 0xBB, // SE V1, 0xBB
		0x41, 0xBB, // SNE V1, 0xBB
		0x51, 0x20, // SE V1, V2
		0x61, 0xBB, // LD V1, 0xBB
		0x71, 0xBB, // ADD V1, 0xBB
		0x81, 0x20, // LD V1, V2
		0x81, 0x21, // OR V1, V2
		0x81, 0x22, // AND V1, V2
		0x81, 0x23, // XOR V1, V2
		0x81, 0x24, // ADD V1, V2
		0x81, 0x25, // SUB V1, V2
		0x81, 0x26, // SHR V1 {, V2}
		0x81, 0x27, // SUBN V1, V2
		0x81, 0x2E, // SHL V1 {, V2}
		0x91, 0x20, // SNE V1, V2
		0xA2, 0x00, // LD I, 200
		0xC1, 0xBB, // RND V1, 0xBB
		0xD1, 0x2F, // DRW V1, V2, 15
		0xE1, 0x9E, // SKP V1
		0xE1, 0xA1, // SKNP V1
		0xF1, 0x07, // LD V1, DT
		0xF1, 0x0A, // LD V1, K
		0xF1, 0x15, // LD DT, V1
		0xF1, 0x18, // LD ST, V1
		0xF1, 0x1E, // ADD I, V1
		0xF1, 0x29, // LD F, V1
		0xF1, 0x33, // LD B, V1
		0xF1, 0x55, // LD [I], V1
		0xF1, 0x65, // LD V1, [I]
		0x00, 0xEE, // RET
		0xB2, 0x00, // JP V0, 200
	};

	std::string s( program.begin(), program.end() );
	std::istringstream iss( s );

	Disassembler dis( "Testit", 0x200 );

	dis.read_binary( iss );
	dis.disassemble();
	dis.collect_data_bytes();

	std::ostringstream oss;
	dis.write_listing( oss );

	std::string expected_output = R"(0000: 00E0  CLS)";
	EXPECT_EQ(oss.str().substr(0, 12), expected_output);
}
