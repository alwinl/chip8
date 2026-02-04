/*
 * decoder_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "decoder.h"

class DecoderTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}

	Decoder decoder;
};

TEST_F(DecoderTest, decode_system_and_flow)
{
	std::stringstream ss;

	auto expect = [&](uint16_t opcode, const char* text) {
		ss.str("");
		ss << decoder.decode(0x000, opcode).instruction;
		EXPECT_EQ(ss.str(), text);
	};

    expect( 0x00E0, "Opcode: CLS Operands: []" );
    expect( 0x00EE, "Opcode: RET Operands: []" );
    expect( 0x1234, "Opcode: JP Operands: [0x234]" );
    expect( 0x2DEF, "Opcode: CALL Operands: [0xDEF]" );
    expect( 0x34AA, "Opcode: SE Operands: [V4, #170]" );
    expect( 0x44AA, "Opcode: SNE Operands: [V4, #170]" );
    expect( 0x5370, "Opcode: SE Operands: [V3, V7]" );
    expect( 0x9370, "Opcode: SNE Operands: [V3, V7]" );
    expect( 0xB370, "Opcode: JP V0 Operands: [0x370]" );
}

TEST_F(DecoderTest, decode_register_immediates)
{
	std::stringstream ss;

	auto expect = [&](uint16_t opcode, const char* text) {
		ss.str("");
		ss << decoder.decode(0x000, opcode).instruction;
		EXPECT_EQ(ss.str(), text);
	};

    expect( 0x6020, "Opcode: LD Operands: [V0, #32]" );
    expect( 0x71AA, "Opcode: ADD Operands: [V1, #170]" );
    expect( 0xA370, "Opcode: LD I Operands: [0x370]" );
}

TEST_F(DecoderTest, decode_arithmetic)
{
	std::stringstream ss;

	auto expect = [&](uint16_t opcode, const char* text) {
		ss.str("");
		ss << decoder.decode(0x000, opcode).instruction;
		EXPECT_EQ(ss.str(), text);
	};

    expect( 0x8370, "Opcode: LD Operands: [V3, V7]" );
    expect( 0x8371, "Opcode: OR Operands: [V3, V7]" );
    expect( 0x8372, "Opcode: AND Operands: [V3, V7]" );
    expect( 0x8373, "Opcode: XOR Operands: [V3, V7]" );
    expect( 0x8374, "Opcode: ADD Operands: [V3, V7]" );
    expect( 0x8375, "Opcode: SUB Operands: [V3, V7]" );
    expect( 0x8376, "Opcode: SHR Operands: [V3, V7]" );
    expect( 0x8377, "Opcode: SUBN Operands: [V3, V7]" );
    expect( 0x837E, "Opcode: SHL Operands: [V3, V7]" );
}

TEST_F(DecoderTest, decode_drawing_and_keys)
{
	std::stringstream ss;

	auto expect = [&](uint16_t opcode, const char* text) {
		ss.str("");
		ss << decoder.decode(0x000, opcode).instruction;
		EXPECT_EQ(ss.str(), text);
	};

    expect( 0xD45F, "Opcode: DRW Operands: [V4, V5, 0xF]" );
    expect( 0xE99E, "Opcode: SKP Operands: [K9]" );
    expect( 0xE8A1, "Opcode: SKNP Operands: [K8]" );
}

TEST_F(DecoderTest, decode_fx_instructions)
{
	std::stringstream ss;

	auto expect = [&](uint16_t opcode, const char* text) {
		ss.str("");
		ss << decoder.decode(0x000, opcode).instruction;
		EXPECT_EQ(ss.str(), text);
	};

    expect( 0xC1AA, "Opcode: RND Operands: [V1, #170]" );
    expect( 0xF207, "Opcode: ST DT Operands: [V2]" );
    expect( 0xF20A, "Opcode: ST K Operands: [V2]" );
    expect( 0xF215, "Opcode: LD DT Operands: [V2]" );
    expect( 0xF218, "Opcode: LD ST Operands: [V2]" );
    expect( 0xF21E, "Opcode: ADD I Operands: [V2]" );
    expect( 0xF229, "Opcode: LD F Operands: [V2]" );
    expect( 0xF233, "Opcode: LD B Operands: [V2]" );
    expect( 0xF255, "Opcode: ST [I] Operands: [V0-V2]" );
    expect( 0xF265, "Opcode: LD [I] Operands: [V0-V2]" );
}

TEST_F(DecoderTest, decode_invalids)
{
	std::stringstream ss;

	auto expect = [&](uint16_t opcode, const char* text) {
		ss.str("");
		ss << decoder.decode(0x000, opcode).instruction;
		EXPECT_EQ(ss.str(), text) << opcode;
	};

    expect( 0x0000, "Opcode: NOP Operands: []" );
    expect( 0x8FF8, "Opcode: NOP Operands: []" );
    expect( 0x9001, "Opcode: NOP Operands: []" );
    expect( 0xE99F, "Opcode: NOP Operands: []" );
    expect( 0xFFFF, "Opcode: NOP Operands: []" );
    expect( 0xF045, "Opcode: NOP Operands: []" );
}

TEST_F(DecoderTest, decode_invalids_generated)
{
    std::stringstream ss;

    auto expect_nop = [&](uint16_t opcode) {
        ss.str("");
        ss << decoder.decode(0x000, opcode).instruction;
        EXPECT_EQ(ss.str(), "Opcode: NOP Operands: []") << " Failed for opcode: 0x"
                                                      << std::hex << std::uppercase << opcode;
    };

    // Each entry defines a family: mask & allowed low-nibble / byte values
    struct OpcodeFamily {
        uint16_t high_mask;                // mask to extract high nibble(s)
        uint16_t high_bits;                // value of high nibble(s) for this family
        std::vector<uint16_t> valid_suffixes; // allowed low-nibble / byte
    };

    const std::vector<OpcodeFamily> families = {
        // 0x0NNN family (only 00E0, 00EE, 0000 are valid)
        { 0xF000, 0x0000, {0x0000, 0x00E0, 0x00EE} },
        // 8XYN ALU family (low nibble must be 0..7 or E)
        { 0xF000, 0x8000, {0x0,1,2,3,4,5,6,7,0xE} },
        // 9XY0 SNE (low nibble must be 0)
        { 0xF000, 0x9000, {0x0} },
        // EX9E / EXA1 (exact low byte)
        { 0xF000, 0xE000, {0x9E, 0xA1} },
        // FX?? (only valid specific suffixes)
        { 0xF000, 0xF000, {0x07,0x0A,0x15,0x18,0x1E,0x29,0x33,0x55,0x65} },
    };

    for (const auto& fam : families)
    {
        // iterate all possible 12-bit opcode suffixes
        for (uint16_t candidate = 0; candidate <= 0x0FFF; ++candidate)
        {
            // extract the low bits relevant for this family
            uint16_t low = candidate & 0x00FF; // default: low byte
            if (fam.high_bits == 0x8000 || fam.high_bits == 0x9000) {
                low = candidate & 0x000F; // ALU / 9xy uses low nibble
            }

            // skip valid suffixes
            if (std::find(fam.valid_suffixes.begin(), fam.valid_suffixes.end(), low) != fam.valid_suffixes.end())
                continue;

            // construct opcode
            uint16_t opcode = (fam.high_bits & fam.high_mask) | (candidate & ~fam.high_mask);
            expect_nop(opcode);
        }
    }
}

TEST_F(DecoderTest, control_flow_unconditional)
{
    auto r = decoder.decode(0x200, 0x1234); // JP 0x234

    ASSERT_TRUE(r.target.has_value());
    EXPECT_EQ(r.target->address, 0x234);

    EXPECT_EQ(r.next_addresses.size(), 1);
    EXPECT_EQ(r.next_addresses[0], 0x234);
}

TEST_F(DecoderTest, control_flow_call)
{
    auto r = decoder.decode(0x200, 0x2ABC); // CALL 0xABC

    ASSERT_TRUE(r.target.has_value());
    EXPECT_EQ(r.target->address, 0xABC);

    EXPECT_EQ(r.next_addresses.size(), 2);
    EXPECT_EQ(r.next_addresses[0], 0x202);
    EXPECT_EQ(r.next_addresses[1], 0xABC);
}

TEST_F(DecoderTest, control_flow_ret)
{
    auto r = decoder.decode(0x200, 0x00EE); // RET

    EXPECT_FALSE(r.target.has_value());
    EXPECT_TRUE(r.next_addresses.empty());
}

TEST_F(DecoderTest, control_flow_skip)
{
    // SE V1, #AA
    auto r = decoder.decode(0x300, 0x31AA);

    EXPECT_FALSE(r.target.has_value());

    ASSERT_EQ(r.next_addresses.size(), 2);
    EXPECT_EQ(r.next_addresses[0], 0x302); // fallthrough
    EXPECT_EQ(r.next_addresses[1], 0x304); // skip
}

TEST_F(DecoderTest, control_flow_key_skip)
{
    // SKP V9
    auto r = decoder.decode(0x400, 0xE99E);

    EXPECT_FALSE(r.target.has_value());

    ASSERT_EQ(r.next_addresses.size(), 2);
    EXPECT_EQ(r.next_addresses[0], 0x402);
    EXPECT_EQ(r.next_addresses[1], 0x404);
}

TEST_F(DecoderTest, control_flow_fallthrough_only)
{
    auto r = decoder.decode(0x500, 0x8374); // ADD V3, V7

    EXPECT_FALSE(r.target.has_value());

    ASSERT_EQ(r.next_addresses.size(), 1);
    EXPECT_EQ(r.next_addresses[0], 0x502);
}

TEST_F(DecoderTest, control_flow_invalid_opcode)
{
    auto r = decoder.decode(0x600, 0xFFFF);

    EXPECT_FALSE(r.target.has_value());

    ASSERT_EQ(r.next_addresses.size(), 1);
    EXPECT_EQ(r.next_addresses[0], 0x602);
}
