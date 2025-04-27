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

#include "commandlineparser.h"

#include <algorithm>

#include <gtest/gtest.h>

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

class CommandLineParserTest : public ::testing::Test {
protected:
	void SetUp() override {
		
	}
	void TearDown() override {
	}
};

TEST_F(CommandLineParserTest, ValidCreation)
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	CommandLineParser filenames( argument_list );
	
	EXPECT_EQ( basename + ".src", filenames.get_source_name() );
	EXPECT_EQ( basename + ".bin", filenames.get_binary_name() );
	EXPECT_EQ( "", filenames.get_listing_name() );
}

TEST_F(CommandLineParserTest, Help)
{
	CommandLineParser args( { "-h" } );
}

TEST_F(CommandLineParserTest, InvalidCreation)
{
	std::vector<std::string> argument_list = {};
	try {
		CommandLineParser filenames( argument_list );
	} catch( std::invalid_argument const &ex ) {
		return;
	}

	FAIL() << "Constructor should have thrown";
}

TEST_F(CommandLineParserTest, OldStyleArgsCreation)
{
	std::string prog_name = "chasm8";
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = basename + "." + random_string( 3 );
	std::string listing_name = basename + "." + random_string( 3 );

	std::vector<std::string> args = {
		prog_name, "-o", binary_name, "--listing=" + listing_name, source_name
	};

	std::vector<char*> argv_ptrs;
	argv_ptrs.reserve(args.size()); // +1 for a fake program name

	for (std::string& arg : args)
		argv_ptrs.push_back(arg.data());

	int argc = static_cast<int>(argv_ptrs.size());
	char** argv = argv_ptrs.data();

	CommandLineParser filenames( argc, argv );

	EXPECT_EQ( source_name, filenames.get_source_name() ) << "Not the right source file name: " << source_name << " != " << filenames.get_source_name();
	EXPECT_EQ( binary_name, filenames.get_binary_name() ) << "Not the right binary file name: " << binary_name << " != " << filenames.get_binary_name();
	EXPECT_EQ( listing_name, filenames.get_listing_name() ) << "Not the right listing file name: " << listing_name << " != " << filenames.get_listing_name();
}

TEST_F(CommandLineParserTest, GetSourceTest)
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	CommandLineParser filenames( argument_list );

	EXPECT_EQ( basename + ".src", filenames.get_source_name() );
}

TEST_F(CommandLineParserTest, GetBinaryFromArgTest)
{
	std::string source_basename = random_string( 8 );
	std::string binary_basename = random_string( 8 );

	std::vector<std::string> argument_list = { "-o", binary_basename + ".bin", source_basename + ".src" };
	CommandLineParser filenames( argument_list );

	EXPECT_EQ( binary_basename + ".bin", filenames.get_binary_name() );
}

TEST_F(CommandLineParserTest, GetBinaryFromSourceTest)
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	CommandLineParser filenames( argument_list );

	EXPECT_EQ( basename + ".bin", filenames.get_binary_name() );
}

TEST_F(CommandLineParserTest, GetListingFromArgTest)
{
	std::string source_basename = random_string( 8 );
	std::string binary_basename = random_string( 8 );
	std::string listing_basename = random_string( 8 );

	std::vector<std::string> argument_list = { "-o", binary_basename + ".bin", "--listing=" + listing_basename + ".lst", source_basename + ".src" };
	CommandLineParser filenames( argument_list );

	EXPECT_EQ( listing_basename + ".lst", filenames.get_listing_name() );
}

TEST_F(CommandLineParserTest, GetBlankListingFromSourceTest)
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { basename + ".src" };
	CommandLineParser filenames( argument_list );

	EXPECT_EQ( "", filenames.get_listing_name() );

}

TEST_F(CommandLineParserTest, GetDefaultListingFromSourceTest)
{
	std::string basename = random_string( 8 );

	std::vector<std::string> argument_list = { "-l", basename + ".src" };
	CommandLineParser filenames( argument_list );

	EXPECT_EQ( basename + ".src", filenames.get_source_name() );
	EXPECT_EQ( basename + ".bin", filenames.get_binary_name() );
	EXPECT_EQ( basename + ".lst", filenames.get_listing_name() );
}

TEST_F(CommandLineParserTest, GetAllFromOneArgument)
{
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = basename + ".bin";
	// std::string listing_name = basename + ".lst";

	std::vector<std::string> argument_list = { source_name };

	CommandLineParser filenames( argument_list );

	EXPECT_EQ( source_name, filenames.get_source_name() );
	EXPECT_EQ( binary_name, filenames.get_binary_name() );
	EXPECT_EQ( "", filenames.get_listing_name() );
}

TEST_F(CommandLineParserTest, GetAllFromTwoArguments)
{
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = random_string( 8 ) + "." + random_string( 3 );
	// std::string listing_name = basename + ".lst";

	std::vector<std::string> argument_list = { "-o", binary_name, source_name };

	CommandLineParser filenames( argument_list );

	EXPECT_EQ( source_name, filenames.get_source_name() );
	EXPECT_EQ( binary_name, filenames.get_binary_name() );
	EXPECT_EQ( "", filenames.get_listing_name() );
}

	// std::string source_name = random_string( 8 ) + "." + random_string( 3 );
	// std::string binary_name = random_string( 8 ) + "." + random_string( 3 );
	// std::string listing_name = random_string( 8 ) + "." + random_string( 3 );

TEST_F(CommandLineParserTest, GetAllFromThreeArguments)
{
	std::string source_name = "source." + random_string( 3 );
	std::string binary_name = "binary." + random_string( 3 );
	std::string listing_name = "list." + random_string( 3 );

	std::vector<std::string> argument_list = { "-o", binary_name, "--listing=" + listing_name, source_name };

	CommandLineParser filenames( argument_list );

	EXPECT_EQ( source_name, filenames.get_source_name() );
	EXPECT_EQ( binary_name, filenames.get_binary_name() );
	EXPECT_EQ( listing_name, filenames.get_listing_name() );
}

TEST_F(CommandLineParserTest, GetDefaultListName)
{
	std::string basename = random_string( 8 );

	std::string source_name = basename + "." + random_string( 3 );
	std::string binary_name = random_string( 8 ) + "." + random_string( 3 );
	std::string listing_name = basename + ".lst";

	std::vector<std::string> argument_list = { "-lo", binary_name, source_name };

	CommandLineParser filenames( argument_list );

	EXPECT_EQ( source_name, filenames.get_source_name() );
	EXPECT_EQ( binary_name, filenames.get_binary_name() );
	EXPECT_EQ( listing_name, filenames.get_listing_name() );
}
