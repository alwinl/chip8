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
#include "disassembler/disassembler.h"

#include "disassembler/vectorstream.h"

TEST(IntDisassembler, straight_line_with_data)
{
    std::vector<uint8_t> bin = {
        0x60, 0x01,
        0x61, 0x02,
        0xFF, 0xFF,
        0xAA, 0xBB
    };

	vectorstream is( bin );

    auto [name, ir, symbols] = disassembler::build_ir( is, "test_program", 0x200 );

    ASSERT_EQ(ir.elements.size(), 3);

    // Instruction 1
    auto* i0 = std::get_if<IRInstruction>(&ir.elements[0]);
    ASSERT_NE(i0, nullptr);
    EXPECT_EQ(i0->address, 0x200);

    // Instruction 2
    auto* i1 = std::get_if<IRInstruction>(&ir.elements[1]);
    ASSERT_NE(i1, nullptr);
    EXPECT_EQ(i1->address, 0x202);

    // Data
    auto* d = std::get_if<IRDataBlock>(&ir.elements[2]);
    ASSERT_NE(d, nullptr);
    EXPECT_EQ(d->address, 0x204);
    EXPECT_EQ(d->bytes.size(), 4);
}

TEST(IntDisassembler, conditional_skip)
{
    std::vector<uint8_t>  bin = {
        0x30, 0x00,
        0x60, 0x01,
        0x60, 0x02
    };

	vectorstream is( bin );

    auto [name, ir, symbols] = disassembler::build_ir( is, "test_program", 0x200 );

    // All three instructions must be discovered
    ASSERT_EQ(ir.elements.size(), 3);

    auto* i0 = std::get_if<IRInstruction>(&ir.elements[0]);
    auto* i1 = std::get_if<IRInstruction>(&ir.elements[1]);
    auto* i2 = std::get_if<IRInstruction>(&ir.elements[2]);

    ASSERT_NE(i0, nullptr);
    ASSERT_NE(i1, nullptr);
    ASSERT_NE(i2, nullptr);

    EXPECT_EQ(i0->address, 0x200);
    EXPECT_EQ(i1->address, 0x202);
    EXPECT_EQ(i2->address, 0x204);
}

TEST(IntDisassembler, jump_skips_data)
{
    std::vector<uint8_t>  bin = {
        0x12, 0x06,
        0xAA, 0xBB,
        0xCC, 0xDD,
        0x60, 0x01
    };

	vectorstream is( bin );

    auto [name, ir, symbols] = disassembler::build_ir( is, "test_program", 0x200 );

    ASSERT_EQ(ir.elements.size(), 3);

    auto* i0 = std::get_if<IRInstruction>(&ir.elements[0]);
    auto* d  = std::get_if<IRDataBlock>(&ir.elements[1]);
    auto* i1 = std::get_if<IRInstruction>(&ir.elements[2]);

    ASSERT_NE(i0, nullptr);
    ASSERT_NE(d, nullptr);
    ASSERT_NE(i1, nullptr);

    EXPECT_EQ(i0->address, 0x200);
    EXPECT_EQ(d->address, 0x202);
    EXPECT_EQ(d->bytes.size(), 4);
    EXPECT_EQ(i1->address, 0x206);
}

TEST(IntDisassembler, no_overlapping_elements)
{
    std::vector<uint8_t>  bin = {
        0x12, 0x06,
        0xAA, 0xBB,
        0xCC, 0xDD,
        0x60, 0x01
    };

	vectorstream is( bin );

    auto [name, ir, symbols] = disassembler::build_ir( is, "test_program", 0x200 );

    std::set<uint16_t> covered;

    for (auto& el : ir.elements) {
        std::visit([&](auto& e) {
            if constexpr (std::is_same_v<std::decay_t<decltype(e)>, IRInstruction>) {
                EXPECT_TRUE(covered.insert(e.address).second);
                EXPECT_TRUE(covered.insert(e.address + 1).second);
            } else {
                for (size_t i = 0; i < e.bytes.size(); ++i)
                    EXPECT_TRUE(covered.insert(e.address + i).second);
            }
        }, el);
    }
}

TEST(IntDisassembler, invalid_opcode_terminates_code_path)
{
    std::vector<uint8_t>  bin = {
        0x60, 0x01,
        0x61, 0x02,
        0xFF, 0xFF
    };

	vectorstream is( bin );

    auto [name, ir, symbols] = disassembler::build_ir( is, "test_program", 0x200 );

    ASSERT_EQ(ir.elements.size(), 3);

    EXPECT_TRUE(std::holds_alternative<IRInstruction>(ir.elements[0]));
    EXPECT_TRUE(std::holds_alternative<IRInstruction>(ir.elements[1]));
    EXPECT_TRUE(std::holds_alternative<IRDataBlock>(ir.elements[2]));
}
