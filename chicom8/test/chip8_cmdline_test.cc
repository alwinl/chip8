/*
 * compiler_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "commandlineparser.h"

#include <gtest/gtest.h>

TEST(Chicom8_CommandLineParserTest, ParsesBasicArguments)
{
    std::vector<std::string> args = {
        "--verbose",
        "-o", "out.ch8",
        "program.c8c"
    };

    CommandLineParser parser(args);

    EXPECT_EQ(parser.get_input_name(), "program.c8c");
    EXPECT_EQ(parser.get_output_name(), "out.ch8");
    EXPECT_TRUE(parser.is_verbose());
    EXPECT_FALSE(parser.show_help());
}

TEST(Chicom8_CommandLineParserTest, UsesDefaultOutputIfNotProvided)
{
    std::vector<std::string> args = {
        "examples/main.c8c"
    };

    CommandLineParser parser(args);

    EXPECT_EQ(parser.get_input_name(), "examples/main.c8c");
    EXPECT_EQ(parser.get_output_name(), "main.ch8");  // From stem of input file
}

TEST(Chicom8_CommandLineParserTest, HandlesHelpFlag)
{
    std::vector<std::string> args = {
        "--help"
    };

    CommandLineParser parser(args);

    EXPECT_EQ(parser.get_input_name(), "");
    EXPECT_EQ(parser.get_output_name(), "");
    EXPECT_FALSE(parser.is_verbose());
    EXPECT_TRUE(parser.show_help());
}