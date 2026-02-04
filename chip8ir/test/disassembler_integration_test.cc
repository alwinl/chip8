/*
 * disassembler_integration_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

class DisassemblerIntegrationTest : public ::testing::Test {
protected:
    Disassembler dis;
    ChidasmCmdLineParser cmd;

    void SetUp() override {
        // cmd.set_origin(0x200);   // adjust if your API differs 0x200 is default in CidamCmdLineParser
        dis.configure(cmd);
    }
};

TEST_F(DisassemblerIntegrationTest, straight_line_with_data)
{
    BinImage bin = {
        0x60, 0x01,
        0x61, 0x02,
        0xFF, 0xFF,
        0xAA, 0xBB
    };

    IRProgram ir = dis.build_ir(bin);

    ASSERT_EQ(ir.elements.size(), 3);

    // Instruction 1
    auto* i0 = std::get_if<InstructionElement>(&ir.elements[0]);
    ASSERT_NE(i0, nullptr);
    EXPECT_EQ(i0->address, 0x200);

    // Instruction 2
    auto* i1 = std::get_if<InstructionElement>(&ir.elements[1]);
    ASSERT_NE(i1, nullptr);
    EXPECT_EQ(i1->address, 0x202);

    // Data
    auto* d = std::get_if<DataElement>(&ir.elements[2]);
    ASSERT_NE(d, nullptr);
    EXPECT_EQ(d->address, 0x204);
    EXPECT_EQ(d->byte_run.size(), 4);
}

TEST_F(DisassemblerIntegrationTest, conditional_skip)
{
    BinImage bin = {
        0x30, 0x00,
        0x60, 0x01,
        0x60, 0x02
    };

    IRProgram ir = dis.build_ir(bin);

    // All three instructions must be discovered
    ASSERT_EQ(ir.elements.size(), 3);

    auto* i0 = std::get_if<InstructionElement>(&ir.elements[0]);
    auto* i1 = std::get_if<InstructionElement>(&ir.elements[1]);
    auto* i2 = std::get_if<InstructionElement>(&ir.elements[2]);

    ASSERT_NE(i0, nullptr);
    ASSERT_NE(i1, nullptr);
    ASSERT_NE(i2, nullptr);

    EXPECT_EQ(i0->address, 0x200);
    EXPECT_EQ(i1->address, 0x202);
    EXPECT_EQ(i2->address, 0x204);
}

TEST_F(DisassemblerIntegrationTest, jump_skips_data)
{
    BinImage bin = {
        0x12, 0x06,
        0xAA, 0xBB,
        0xCC, 0xDD,
        0x60, 0x01
    };

    IRProgram ir = dis.build_ir(bin);

    ASSERT_EQ(ir.elements.size(), 3);

    auto* i0 = std::get_if<InstructionElement>(&ir.elements[0]);
    auto* d  = std::get_if<DataElement>(&ir.elements[1]);
    auto* i1 = std::get_if<InstructionElement>(&ir.elements[2]);

    ASSERT_NE(i0, nullptr);
    ASSERT_NE(d, nullptr);
    ASSERT_NE(i1, nullptr);

    EXPECT_EQ(i0->address, 0x200);
    EXPECT_EQ(d->address, 0x202);
    EXPECT_EQ(d->byte_run.size(), 4);
    EXPECT_EQ(i1->address, 0x206);
}

TEST_F(DisassemblerIntegrationTest, no_overlapping_elements)
{
    BinImage bin = {
        0x12, 0x06,
        0xAA, 0xBB,
        0xCC, 0xDD,
        0x60, 0x01
    };

    IRProgram ir = dis.build_ir(bin);

    std::set<uint16_t> covered;

    for (auto& el : ir.elements) {
        std::visit([&](auto& e) {
            if constexpr (std::is_same_v<std::decay_t<decltype(e)>, InstructionElement>) {
                EXPECT_TRUE(covered.insert(e.address).second);
                EXPECT_TRUE(covered.insert(e.address + 1).second);
            } else {
                for (size_t i = 0; i < e.byte_run.size(); ++i)
                    EXPECT_TRUE(covered.insert(e.address + i).second);
            }
        }, el);
    }
}

TEST_F(DisassemblerIntegrationTest, invalid_opcode_terminates_code_path)
{
    BinImage bin = {
        0x60, 0x01,
        0x61, 0x02,
        0xFF, 0xFF
    };

    IRProgram ir = dis.build_ir(bin);

    ASSERT_EQ(ir.elements.size(), 3);

    EXPECT_TRUE(std::holds_alternative<InstructionElement>(ir.elements[0]));
    EXPECT_TRUE(std::holds_alternative<InstructionElement>(ir.elements[1]));
    EXPECT_TRUE(std::holds_alternative<DataElement>(ir.elements[2]));
}
