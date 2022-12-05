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

void ProgramParserTest::TestClearScreen()
{
	std::string input = "cls\nret\n";
	std::stringstream is( input );

	Program prog;

	prog.read_source( is );

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

	CPPUNIT_ASSERT( expected == actual );
}

void ProgramParserTest::TestJP()
{
	std::string input = "\tjp 0x212\n";
	std::stringstream is( input );

	Program prog;

	prog.read_source( is );

	std::ofstream outfile( "temp.hex", std::ios_base::binary | std::ios_base::out );
	prog.write_binary( outfile );
	outfile.close();

	std::ifstream result_file("temp.hex", std::ios_base::binary | std::ios_base::in );
	std::istream_iterator<uint8_t> file_end;
	std::istream_iterator<uint8_t> file_begin(result_file);

	std::vector<uint8_t> actual;
	std::copy( file_begin, file_end, std::back_inserter(actual) );

	std::remove( "temp.hex" );

	std::vector<uint8_t> expected { 0x12, 0x12 };

	CPPUNIT_ASSERT( expected == actual );
}
