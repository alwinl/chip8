/*
 * instructiontest.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "instruction.h"

#include <gtest/gtest.h>

class InstructionTest : public ::testing::Test {
protected:
    void SetUp() override {
        
    }
    void TearDown() override {
    }
};

TEST_F(InstructionTest, DBInstructionExploded)
{
    SymbolTable sym;
    std::vector<std::string> args = { "0x1", ",", "2", ",", "003", ",", "0xFF" };

    DBInstruction instr(args, sym);

    std::ostringstream output;
    instr.emit_binary(output);

    std::string result = output.str();
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(static_cast<uint8_t>(result[0]), 0x01);
    EXPECT_EQ(static_cast<uint8_t>(result[1]), 0x02);
    EXPECT_EQ(static_cast<uint8_t>(result[2]), 0x03);
    EXPECT_EQ(static_cast<uint8_t>(result[3]), 0xFF);    
}

TEST_F(InstructionTest, DBInstructioncombined)
{
    SymbolTable sym;
    std::vector<std::string> args = { "0x1,2, 003 ,   0xFF " };

    DBInstruction instr(args, sym);

    std::ostringstream output;
    instr.emit_binary(output);

    std::string result = output.str();
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(static_cast<uint8_t>(result[0]), 0x01);
    EXPECT_EQ(static_cast<uint8_t>(result[1]), 0x02);
    EXPECT_EQ(static_cast<uint8_t>(result[2]), 0x03);
    EXPECT_EQ(static_cast<uint8_t>(result[3]), 0xFF);    
}

TEST_F(InstructionTest, ThrowsOnInvalidByteValue)
{
    SymbolTable sym;
    std::vector<std::string> args = { "256" };

    EXPECT_THROW(DBInstruction instr(args, sym), std::runtime_error);
}

TEST_F(InstructionTest, ThrowsOnEmptyArgs)
{
    SymbolTable sym;
    std::vector<std::string> args = {};

    EXPECT_THROW(DBInstruction instr(args, sym), std::runtime_error);
}
