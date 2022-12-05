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

#include "program.h"

#include <algorithm>
#include <map>
#include <vector>
#include <memory>

#include <iostream>

class Interpreter
{
public:
	Interpreter( uint8_t byte1, uint8_t byte2 ) {
		this->byte1 = byte1;
		this->byte2 = byte2;
	}

	virtual void add_arguments( std::string argument ) = 0;

	void push_bytes( std::vector<uint8_t>& target ) {
		target.push_back( byte1 );
		target.push_back( byte2 );
	}
protected:
	virtual ~Interpreter() {};

	uint8_t byte1;
	uint8_t byte2;
};

class SimpleCommand : public Interpreter
{
public:
	SimpleCommand( uint8_t byte1, uint8_t byte2 ) : Interpreter(byte1, byte2) {};

	virtual void add_arguments( std::string argument ) {};
};

class JumpCommand : public Interpreter
{
public:
	JumpCommand( uint8_t byte1, uint8_t byte2 ) : Interpreter(byte1, byte2) {};
	virtual void add_arguments( std::string argument )
	{
	};
};

std::map<std::string, std::shared_ptr<Interpreter>> keyword_dispatcher =
{
	{ "CLS", std::shared_ptr<Interpreter>( new SimpleCommand( 0x00, 0xE0 ) ) },
	{ "RET", std::shared_ptr<Interpreter>( new SimpleCommand( 0x00, 0xEE ) ) }
};

Program::Program()
{
	//ctor
}

const std::string whitespace = " \n\r\t\f\v";
const std::string comment = ";";

void Program::read_source( std::istream& is )
{
	std::string line;

	while( !is.eof() ) {
		getline( is, line );

		if( *(line.end() - 1) == '\r' )			// Remove possible \r (strange Windows convention)
			line.pop_back();

		// remove comments
		size_t comment_start = line.find_first_of( comment );
		if( comment_start != std::string::npos )
			line = line.substr( 0, comment_start );

		if( line.empty() )
			continue;

		size_t mnemonic_start = line.find_first_not_of( whitespace );
		size_t mnemonic_end = line.find_first_of( whitespace, mnemonic_start );

		std::string mnemonic = line.substr( mnemonic_start, mnemonic_end );
		std::transform( mnemonic.begin(), mnemonic.end(), mnemonic.begin(), ::toupper );

		std::string argument;
		size_t argument_start = line.find_first_not_of( whitespace, mnemonic_end );
		if( argument_start != std::string::npos ) {
			size_t argument_end = line.find_first_of( whitespace, argument_start );

			if( argument_end !=  std::string::npos )
				argument = line.substr( argument_start, argument_end );
		}

		std::map<std::string, std::shared_ptr<Interpreter> >::iterator it = keyword_dispatcher.find( mnemonic );

		if( it != keyword_dispatcher.end() ) {
			(*it).second->add_arguments( argument );
			(*it).second->push_bytes( codes );
		}
	}

}

void Program::write_binary( std::ostream& os )
{
	std::for_each( codes.begin(), codes.end(),
		[&os](uint8_t code )
		{
			os.write( reinterpret_cast<char *>(&code), sizeof code );
		}
	);
}

void Program::write_listing( std::ostream& os )
{

}

