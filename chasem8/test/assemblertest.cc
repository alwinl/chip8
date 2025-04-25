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



class AssemblerTest : public ::testing::Test {
protected:
	void SetUp() override {
		
	}
	void TearDown() override {
	}
};

TEST_F(AssemblerTest, RemoveSlashR)
{
	std::string actual( "a string with\n\r" );
	std::string expected( "a string with\n" );

	Assembler().remove_slash_r( actual );

	EXPECT_EQ( expected, actual );
}


TEST_F(AssemblerTest, RemoveComments)
{
	std::string actual( "a string with ; a comment\n" );
	std::string expected( "a string with " );

	Assembler().remove_comments( actual );

	EXPECT_EQ( expected, actual );
}

TEST_F(AssemblerTest, Split)
{
	std::string input = "a string with ; a comment\n";
	std::vector<std::string> expected { "a", "string", "with" };

	Assembler prog;
	std::vector<std::string> actual = prog.split( input );

	EXPECT_EQ( expected.size(), actual.size() );

	for( unsigned int index = 0; index < expected.size(); ++index ) {
		EXPECT_EQ( expected[index], actual[index] );
	}
}

TEST_F(AssemblerTest, Identify)
{
	std::string input = "a string with ; a comment\n";
	std::vector<std::string> expected { "a", "string", "with" };

	Assembler prog;
	std::vector<std::string> actual = prog.split( input );

	EXPECT_EQ( expected.size(), actual.size() );

	for( unsigned int index = 0; index < expected.size(); ++index ) {
		EXPECT_EQ( expected[index], actual[index] );
	}
}

TEST_F(AssemblerTest, ClearScreen)
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
TEST_F(AssemblerTest, JP)
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
