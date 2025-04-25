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

#include "assembler.h"

#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <sstream>

#include <iostream>

class Interpreter
{
public:
	Interpreter( uint8_t byte1, uint8_t byte2 ) : byte1( byte1 ), byte2( byte2 ) {}
	Interpreter( Interpreter &other ) = delete;
	Interpreter( Interpreter &&other ) = delete;

	Interpreter &operator=( Interpreter &other ) = delete;
	Interpreter &operator=( Interpreter &&other ) = delete;

	virtual ~Interpreter() = default;

	virtual void add_arguments( std::string argument ) = 0;

	void push_bytes( std::vector<uint8_t> &target ) const
	{
		target.push_back( byte1 );
		target.push_back( byte2 );
	}

protected:
	void add_address( uint16_t address ) {
		byte1 |= (address >> 8);
		byte2 |= (address & 0xFF );
	}

private:
	uint8_t byte1;
	uint8_t byte2;
};

class SimpleCommand : public Interpreter
{
public:
	SimpleCommand( uint8_t byte1, uint8_t byte2 ) : Interpreter( byte1, byte2 ){};

	void add_arguments( std::string argument ) override{};
};

class JumpCommand : public Interpreter
{
public:
	JumpCommand( uint8_t byte1, uint8_t byte2 ) : Interpreter( byte1, byte2 ){};

	void add_arguments( std::string argument ) override
	{
		add_address( 0x212 );
	};
};

std::map<std::string, std::shared_ptr<Interpreter>> keyword_dispatcher = {
	{ "CLS", std::shared_ptr<Interpreter>( new SimpleCommand( 0x00, 0xE0 ) ) },
	{ "RET", std::shared_ptr<Interpreter>( new SimpleCommand( 0x00, 0xEE ) ) },
	{ "JP", std::shared_ptr<Interpreter>( new JumpCommand( 0x10, 0x00 ) ) }
};

const std::string whitespace = " \n\r\t\f\v";
const std::string comment = ";";

void Assembler::remove_slash_r( std::string& input )
{
	if( *( input.end() - 1 ) == '\r' ) // Remove possible \r (strange Windows convention)
		input.pop_back();
}

void Assembler::remove_comments( std::string& input )
{
	const size_t comment_start = input.find_first_of( comment );
	if( comment_start != std::string::npos )
		input = input.substr( 0, comment_start );
}

std::vector<std::string> Assembler::split( std::string input )
{
	remove_comments( input );
	
	std::vector<std::string> fields;
	std::stringstream stream( input );
	std::string field;

	while( stream >> field )
		fields.push_back( field );

	return fields;
}

program_parts Assembler::identify( std::vector<std::string> parts )
{
	program_parts result;

	// part one could be a label but for the moment lets assume it is a mnemonic
	result.label = "";
	result.mnemonic = parts[0];
	result.arguments = ( parts.size() > 1 ) ? parts[1] : "";

	return result;
}

void Assembler::read_source( std::istream &source )
{
	std::string line;

	while( !source.eof() ) {
		getline( source, line );

		remove_slash_r( line );
		remove_comments( line );

		if( line.empty() )
			continue;

		std::vector<std::string> parts = split( line );

		program_parts idents = identify( parts );

		std::transform( idents.mnemonic.begin(), idents.mnemonic.end(), idents.mnemonic.begin(), ::toupper );

		auto entry = keyword_dispatcher.find( idents.mnemonic );

		if( entry != keyword_dispatcher.end() ) {
			( *entry ).second->add_arguments( idents.arguments );
			( *entry ).second->push_bytes( codes );
		}
	}
}

void Assembler::write_binary( std::ostream &target )
{
	std::for_each( codes.begin(), codes.end(), [&target]( uint8_t code ) { target << code; } );
}

void Assembler::write_listing( std::ostream &target ) {}