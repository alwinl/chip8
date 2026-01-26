/*
 * ebnf_commandline_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_cmdlineparser.h"

class EBNFCommandLineTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}

    CmdLineParser parser;
};

TEST_F(EBNFCommandLineTest, DecodeNoTarjan)
{
    std::vector<std::string> args = {
        "some_input_file"
    };

	parser.parse_args( args );

    EXPECT_EQ(parser.get_tarjan_name().empty(), true);
}

TEST_F(EBNFCommandLineTest, TarjanToStdOut)
{
    std::vector<std::string> args = {
		"--tarjan",
        "some_input_file"
    };

	parser.parse_args( args );

    EXPECT_EQ(parser.get_tarjan_name(), "-");
}

TEST_F(EBNFCommandLineTest, TarjanToFile)
{
    std::vector<std::string> args = {
		"--tarjan",
		"-o",
		"tarjan.txt",
        "some_input_file"
    };

	parser.parse_args( args );

    EXPECT_EQ(parser.get_tarjan_name(), "tarjan.txt");
}