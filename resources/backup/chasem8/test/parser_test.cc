/*
 * parser_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "parser.h"

TEST(ParserTest, SingleInstruction)
{
    Parser p(std::string("LD V0, 10\n"));
    Program prog = p.parse_all();

    ASSERT_EQ(prog.lines.size(), 1);

    const auto& line = prog.lines[0];

    EXPECT_FALSE(line.label.has_value());
    EXPECT_EQ(line.kind, LineNode::Kind::INSTRUCTION);
    EXPECT_EQ(line.mnemonic, "LD");
    ASSERT_EQ(line.arguments.size(), 2);
    EXPECT_EQ(line.arguments[0], "V0");
    EXPECT_EQ(line.arguments[1], "10");
}

TEST(ParserTest, LabelAndInstruction)
{
    Parser p(std::string("loop: ADD V0, 1\n"));
    Program prog = p.parse_all();

    ASSERT_EQ(prog.lines.size(), 1);

    const auto& line = prog.lines[0];

    ASSERT_TRUE(line.label.has_value());
    EXPECT_EQ(line.label.value(), "loop");

    EXPECT_EQ(line.kind, LineNode::Kind::INSTRUCTION);
    EXPECT_EQ(line.mnemonic, "ADD");
}

TEST(ParserTest, VariableDefinition)
{
    Parser p(std::string("COUNT EQU 10\n"));
    Program prog = p.parse_all();

    ASSERT_EQ(prog.lines.size(), 1);

    const auto& line = prog.lines[0];

    EXPECT_EQ(line.kind, LineNode::Kind::VARIABLE);
    EXPECT_EQ(line.variable_name, "COUNT");
    EXPECT_EQ(line.mnemonic, "EQU");
    ASSERT_EQ(line.arguments.size(), 1);
    EXPECT_EQ(line.arguments[0], "10");
}

TEST(ParserTest, MultipleLines)
{
    Parser p(std::string(
        "start:\n"
        "ADD V0, 1\n"
        "LD V0, 0\n"
    ));

    Program prog = p.parse_all();

    ASSERT_EQ(prog.lines.size(), 3);

    EXPECT_TRUE(prog.lines[0].label.has_value());
    EXPECT_EQ(prog.lines[2].mnemonic, "LD");
    EXPECT_EQ(prog.lines[1].mnemonic, "ADD");
}

TEST(ParserTest, CommentsAndEmptyLines)
{
    Parser p(std::string(
        "; this is a comment\n"
        "\n"
        "LD V0, 1\n"
    ));

    Program prog = p.parse_all();

    ASSERT_EQ(prog.lines.size(), 1);
    EXPECT_EQ(prog.lines[0].mnemonic, "LD");
}
