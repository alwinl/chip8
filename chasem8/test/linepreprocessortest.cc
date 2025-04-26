/*
 * linepreprocessortest.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "linepreprocessor.h"

class LinePreprocessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        
    }
    void TearDown() override {
    }
};

TEST_F( LinePreprocessorTest, RemovesComments )
{
    EXPECT_EQ(LinePreprocessor::tokenise("LD V0, 0xF0 ; This is a comment"), std::vector<std::string>({"LD", "V0,", "0xF0"}));
    EXPECT_TRUE(LinePreprocessor::tokenise("; This is a whole line comment").empty());
    EXPECT_EQ(LinePreprocessor::tokenise("JP 0x200; This is a comment"), std::vector<std::string>({"JP", "0x200"}));
}

TEST_F( LinePreprocessorTest, RemovesCarriageReturn )
{
    EXPECT_EQ(LinePreprocessor::tokenise("CLS\r"), std::vector<std::string>({"CLS"}));
    EXPECT_EQ(LinePreprocessor::tokenise("RET ; comment\r"), std::vector<std::string>({"RET"}));
}


TEST_F( LinePreprocessorTest, TokenisesWord )
{
    std::vector<std::string> expected = {"LD", "V0,", "0xF0"};
    EXPECT_EQ(LinePreprocessor::tokenise("LD V0, 0xF0"), expected);
}

TEST_F( LinePreprocessorTest, TokeniseEmptyLineReturnsEmptyVector)
{
    EXPECT_TRUE(LinePreprocessor::tokenise("").empty());
}

TEST_F( LinePreprocessorTest, HandlesLeadingAndTrailingWhitespace)
{
    std::vector<std::string> expected = {"JP", "0x300"};
    EXPECT_EQ(LinePreprocessor::tokenise("   JP    0x300   "), expected);
}

TEST_F( LinePreprocessorTest, KeepsCommaSeparatedTokenTogether)
{
    std::vector<std::string> tokens = LinePreprocessor::tokenise(".DB 0x12,0x34");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], ".DB");
    EXPECT_EQ(tokens[1], "0x12,0x34");
}