/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
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
 *
 *
 */

#include "filenameextractortest.h"

#include "filenameextractor.h"

#include <algorithm>

CPPUNIT_TEST_SUITE_REGISTRATION( FilenameExtractorTest );

std::string random_string( size_t length )
{
	auto randchar = []() -> char {
		const char charset[] = "0123456789"
							   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							   "abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = ( sizeof( charset ) - 1 );
		return charset[rand() % max_index];
	};
	std::string str( length, 0 );
	std::generate_n( str.begin(), length, randchar );
	return str;
}

void FilenameExtractorTest::ValidCreation()
{
	std::vector<std::string> argument_list = { random_string( 8 ) + ".src" };
	FilenameExtractor filenames( argument_list );
}

void FilenameExtractorTest::InvalidCreation()
{
	std::vector<std::string> argument_list = {};
	try {
		FilenameExtractor filenames( argument_list );
	} catch( std::invalid_argument const &ex ) {
		return;
	}

	CPPUNIT_FAIL( "Constructor should have thrown" );
}

void FilenameExtractorTest::OldStyleArgsCreation()
{
	std::string prog_name = "chasm8";
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = basename + "." + random_string( 3 );
	std::string listing_name = basename + "." + random_string( 3 );

	char *arg0 = new char[prog_name.size() + 1];
	std::copy( prog_name.begin(), prog_name.end(), arg0 );
	arg0[prog_name.size()] = '\0';

	char *arg1 = new char[source_name.size() + 1];
	std::copy( source_name.begin(), source_name.end(), arg1 );
	arg1[source_name.size()] = '\0';

	char *arg2 = new char[binary_name.size() + 1];
	std::copy( binary_name.begin(), binary_name.end(), arg2 );
	arg2[binary_name.size()] = '\0';

	char *arg3 = new char[listing_name.size() + 1];
	std::copy( listing_name.begin(), listing_name.end(), arg3 );
	arg1[listing_name.size()] = '\0';

	char *argv[] = { arg0, arg1, arg2, arg3, NULL };

	FilenameExtractor filenames( 4, argv );

	CPPUNIT_ASSERT_EQUAL_MESSAGE( "Not the right source file name", source_name, filenames.get_source_name() );
	CPPUNIT_ASSERT_EQUAL_MESSAGE( "Not the right binary file name", binary_name, filenames.get_binary_name() );
	CPPUNIT_ASSERT_EQUAL_MESSAGE( "Not the right listing file name", listing_name, filenames.get_listing_name() );
}

void FilenameExtractorTest::GetSourceTest()
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( basename + ".src", filenames.get_source_name() );
}

void FilenameExtractorTest::GetBinaryFromArgTest()
{
	std::string source_basename = random_string( 8 );
	std::string binary_basename = random_string( 8 );

	std::vector<std::string> argument_list = { source_basename + ".src", binary_basename + ".bin" };
	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( binary_basename + ".bin", filenames.get_binary_name() );
}

void FilenameExtractorTest::GetBinaryFromSourceTest()
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( basename + ".bin", filenames.get_binary_name() );
}

void FilenameExtractorTest::GetListingFromArgTest()
{
	std::string source_basename = random_string( 8 );
	std::string binary_basename = random_string( 8 );
	std::string listing_basename = random_string( 8 );

	std::vector<std::string> argument_list = { source_basename + ".src", binary_basename + ".bin",
											   listing_basename + ".lst" };
	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( listing_basename + ".lst", filenames.get_listing_name() );
}

void FilenameExtractorTest::GetListingFromSourceTest()
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( basename + ".lst", filenames.get_listing_name() );
}

void FilenameExtractorTest::GetAllFromOneArgument()
{
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = basename + ".bin";
	std::string listing_name = basename + ".lst";

	std::vector<std::string> argument_list = { source_name };

	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( source_name, filenames.get_source_name() );
	CPPUNIT_ASSERT_EQUAL( binary_name, filenames.get_binary_name() );
	CPPUNIT_ASSERT_EQUAL( listing_name, filenames.get_listing_name() );
}

void FilenameExtractorTest::GetAllFromTwoArguments()
{
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = random_string( 8 ) + "." + random_string( 3 );
	std::string listing_name = basename + ".lst";

	std::vector<std::string> argument_list = { source_name, binary_name };

	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( source_name, filenames.get_source_name() );
	CPPUNIT_ASSERT_EQUAL( binary_name, filenames.get_binary_name() );
	CPPUNIT_ASSERT_EQUAL( listing_name, filenames.get_listing_name() );
}

void FilenameExtractorTest::GetAllFromThreeArguments()
{
	std::string source_name = random_string( 8 ) + "." + random_string( 3 );
	std::string binary_name = random_string( 8 ) + "." + random_string( 3 );
	std::string listing_name = random_string( 8 ) + "." + random_string( 3 );

	std::vector<std::string> argument_list = { source_name, binary_name, listing_name };

	FilenameExtractor filenames( argument_list );

	CPPUNIT_ASSERT_EQUAL( source_name, filenames.get_source_name() );
	CPPUNIT_ASSERT_EQUAL( binary_name, filenames.get_binary_name() );
	CPPUNIT_ASSERT_EQUAL( listing_name, filenames.get_listing_name() );
}