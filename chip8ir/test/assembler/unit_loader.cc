/*
 * unit_loader.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <sstream>
#include <string>

#include "assembler/loader.h"

TEST(UnitLoader, EmptyInput)
{
    std::istringstream is("");

    ASMSource source = load_assembly_source(is);

    EXPECT_TRUE(source.empty());
}

TEST(UnitLoader, SingleLine)
{
    std::istringstream is("CLS");

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 1);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 1);
}

TEST(UnitLoader, MultipleLines)
{
    std::istringstream is(
        "CLS\n"
        "JP 0x200\n"
        "LD V0, 0xFF\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 3);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 1);
    EXPECT_EQ(source[1].text, "JP 0x200");
    EXPECT_EQ(source[1].line_no, 2);
    EXPECT_EQ(source[2].text, "LD V0, 0xFF");
    EXPECT_EQ(source[2].line_no, 3);
}

TEST(UnitLoader, SkipsEmptyLines)
{
    std::istringstream is(
        "CLS\n"
        "\n"
        "JP 0x200\n"
        "\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 1);
    EXPECT_EQ(source[1].text, "JP 0x200");
    EXPECT_EQ(source[1].line_no, 3);
}

TEST(UnitLoader, SkipsFullLineComments)
{
    std::istringstream is(
        "; first comment\n"
        "CLS\n"
        "; second comment\n"
        "JP 0x200\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 2);
    EXPECT_EQ(source[1].text, "JP 0x200");
    EXPECT_EQ(source[1].line_no, 4);
}

TEST(UnitLoader, StripsTrailingComments)
{
    std::istringstream is(
        "CLS ; clear the screen\n"
        "JP 0x200; jump to start\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[1].text, "JP 0x200");
}

TEST(UnitLoader, SkipsLinesContainingOnlyComments)
{
    std::istringstream is(
        "CLS\n"
        "   ; comment after whitespace\n"
        "JP 0x200\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 1);
    EXPECT_EQ(source[1].text, "JP 0x200");
    EXPECT_EQ(source[1].line_no, 3);
}

TEST(UnitLoader, HandlesWindowsLineEndings)
{
    std::istringstream is(
        "CLS\r\n"
        "JP 0x200\r\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 1);
    EXPECT_EQ(source[1].text, "JP 0x200");
    EXPECT_EQ(source[1].line_no, 2);
}

TEST(UnitLoader, PreservesOriginalLineNumbers)
{
    std::istringstream is(
        "; header\n"
        "\n"
        "CLS\n"
        "; comment\n"
        "JP 0x200\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].line_no, 3);
    EXPECT_EQ(source[1].line_no, 5);
}

TEST(UnitLoader, PreservesWhitespaceOnAssemblyLines)
{
    std::istringstream is(
        "    CLS\n"
        "\tJP 0x200\n"
    );

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "    CLS");
    EXPECT_EQ(source[1].text, "\tJP 0x200");
}

TEST(UnitLoader, PreservesLastLineWithoutNewline)
{
    std::istringstream is("CLS\nJP 0x200");

    ASMSource source = load_assembly_source(is);

    ASSERT_EQ(source.size(), 2);
    EXPECT_EQ(source[0].text, "CLS");
    EXPECT_EQ(source[0].line_no, 1);
    EXPECT_EQ(source[1].text, "JP 0x200");
    EXPECT_EQ(source[1].line_no, 2);
}


