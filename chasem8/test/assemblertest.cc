/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#include "assembler.h"

#include <vector>
#include <iterator>
#include <cstdio>
#include <fstream>

#include <gtest/gtest.h>

class vector_streambuf : public std::streambuf
{
public:
	vector_streambuf( std::vector<uint8_t>& buffer ) : buffer(buffer) {};

protected:
	std::streambuf::int_type overflow( std::streambuf::int_type ch ) override
	{
		if( ch != EOF )
			buffer.push_back( static_cast<uint8_t>(ch));

		return ch;
	}

    std::streamsize xsputn(const char* s, std::streamsize count) override
	{
        buffer.insert( buffer.end(), s, s + count );
        return count;
    }
private:
	std::vector<uint8_t>& buffer;
};

TEST(Chasem8_AssemblerTest, ClearScreen)
{
	std::string input = "cls\nret\n";
	std::stringstream source( input );

	Assembler prog;

	prog.read_source( source );

	std::vector<uint8_t> actual;
	vector_streambuf vsbuf( actual );
	std::ostream output( &vsbuf );

	prog.write_binary( output );

	std::vector<uint8_t> expected { 0x00, 0xE0, 0x00, 0xEE };

	EXPECT_EQ( expected.size(), actual.size() );

	for( unsigned int index = 0; index < expected.size(); ++index ) {
		EXPECT_EQ( expected[index], actual[index] );
	}
}

TEST(Chasem8_AssemblerTest, JP)
{
	std::string input = "\tjp 0x212\n";
	std::stringstream source( input );

	Assembler prog;

	prog.read_source( source );

	std::vector<uint8_t> actual;
	vector_streambuf vsbuf( actual );
	std::ostream output( &vsbuf );

	prog.write_binary( output );

	std::vector<uint8_t> expected { 0x12, 0x12 };

	EXPECT_EQ( expected.size(), actual.size() );

	for( unsigned int index = 0; index < expected.size(); ++index ) {
		EXPECT_EQ( expected[index], actual[index] );
	}
}

class AssemblerTestInterface: public Assembler
{
public:
	using Assembler::extract_label;
    using Assembler::extract_instruction;
    using Assembler::get_instruction_count;
    using Assembler::get_symbol;
};

TEST(Chasem8_AssemblerTest, ExtractsLabelAndRemovesItFromTokens)
{
    AssemblerTestInterface assembler;
    std::vector<std::string> tokens = { "start:", "JP", "0x300" };
    assembler.extract_label(tokens, 0x200);

    // Assert label added
    EXPECT_EQ(assembler.get_symbol("start"), 0x200);
    // Assert label removed
    ASSERT_EQ(tokens[0], "JP");
}

TEST(Chasem8_AssemblerTest, AddsInstructionAndReturnsCorrectLength)
{
    AssemblerTestInterface assembler;
    std::vector<std::string> tokens = { "CLS" };
    uint16_t len = assembler.extract_instruction(tokens);

    EXPECT_EQ(len, 2); // assuming CLS is 2 bytes
    EXPECT_EQ(assembler.get_instruction_count(), 1);
}
