/*
 * asm_loader.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <istream>
#include <string>
#include <cassert>

#include "chip8formats.h"

class ASMSourceLoader
{
public:
	ASMSourceLoader() = default;

	ASMSource load( std::istream& is )
	{
		ASMSource source {};
		std::string line;
		size_t line_no = 0;

		while( std::getline( is, line ) ) {

			line_no++;

			if( line.empty() || strip_trailing_cr( line ) || strip_comments( line ) )
				continue;

			source.push_back( {line, line_no} );
		}

		return source;
	};

private:
	bool strip_trailing_cr( std::string &line )
	{
		assert( !line.empty() && "strip_trailing_cr requires non-empty line" );

		if( line.back() == '\r' )
			line.pop_back();

		return line.empty();
	}

	bool strip_comments( std::string &line )
	{
		assert( !line.empty() && "strip_comments requires non-empty line" );

		const size_t comment_start = line.find_first_of(';');

		if( comment_start != std::string::npos )
			line = line.substr(0, comment_start);

		return line.empty();
	}
};
