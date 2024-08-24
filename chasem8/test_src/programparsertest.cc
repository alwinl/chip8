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

#include "programparsertest.h"

#include "program.h"

#include <vector>
#include <iterator>
#include <cstdio>

CPPUNIT_TEST_SUITE_REGISTRATION( ProgramParserTest );


void ProgramParserTest::TestRemoveSlashR( )
{
	std::string actual( "a string with\n\r" );
	std::string expected( "a string with\n" );

	Program().remove_slash_r( actual );

	CPPUNIT_ASSERT_EQUAL( expected, actual );

}



void ProgramParserTest::TestRemoveComments()
{

}

void ProgramParserTest::TestSplit()
{

}

void ProgramParserTest::TestIdentify()
{

}

void ProgramParserTest::TestClearScreen()
{
	std::string input = "cls\nret\n";
	std::stringstream source( input );

	Program prog;

	prog.read_source( source );

	std::ofstream outfile( "temp.hex", std::ios_base::binary | std::ios_base::out );
	prog.write_binary( outfile );
	outfile.close();

	std::ifstream result_file("temp.hex", std::ios_base::binary | std::ios_base::in );
	std::istream_iterator<uint8_t> file_end;
	std::istream_iterator<uint8_t> file_begin(result_file);

	std::vector<uint8_t> actual;
	std::copy( file_begin, file_end, std::back_inserter(actual) );

	std::remove( "temp.hex" );

	std::vector<uint8_t> expected { 0x00, 0xE0, 0x00, 0xEE };

	CPPUNIT_ASSERT_EQUAL_MESSAGE( "Not the expected size", expected.size(), actual.size() );

	for( unsigned int index = 0; index < expected.size(); ++index ) {
		std::string message = "Byte mismatch at index " + std::to_string( index );
		CPPUNIT_ASSERT_EQUAL_MESSAGE( message, expected[index], actual[index] );
	}
}

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

    std::streamsize xsputn(const char* s, std::streamsize count) override {
        buffer.insert( buffer.end(), s, s + count );
        return count;
    }
private:
	std::vector<uint8_t>& buffer;

};

void ProgramParserTest::TestJP()
{
	std::string input = "\tjp 0x212\n";
	std::stringstream source( input );

	Program prog;

	prog.read_source( source );

	std::vector<uint8_t> actual;
	vector_streambuf vsbuf( actual );
	std::ostream output( &vsbuf );

	prog.write_binary( output );

	std::vector<uint8_t> expected { 0x12, 0x12 };

	CPPUNIT_ASSERT_EQUAL_MESSAGE( "Not the expected size", expected.size(), actual.size() );

	for( unsigned int index = 0; index < expected.size(); ++index ) {
		std::string message = "Byte mismatch at index " + std::to_string( index );
		CPPUNIT_ASSERT_EQUAL_MESSAGE( message, expected[index], actual[index] );
	}
}
