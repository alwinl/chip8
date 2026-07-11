/*
 * ir_builder.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir_builder.h"
#include "chip8_ast.h"

#include <ostream>

std::string IRBuilder::new_label(const std::string& stem)
{
    int& counter = label_map[stem]; // default-initializes to 0
    return stem + std::to_string(counter++);
}

void IRBuilder::emit_jump_if_false( Expr *expr, std::string label )
{
	std::string line = std::string( "JumpIfFalse " );

	// line += "cond";
	// line += " ";
	line += label;

	lines.push_back( line );
}

void IRBuilder::emit_jump( std::string label )
{
	std::string line = std::string( "JP " );
	line += label;
	lines.push_back( line );
}

void IRBuilder::emit_label( std::string label )
{
	lines.push_back( label + ":" );
}

void IRBuilder::emit_return()
{
	lines.push_back( "Return" );
}

void IRBuilder::emit_comment( std::string comment )
{
	lines.push_back( comment );
}

void IRBuilder::emit_draw( std::string sprite_name, Expr *x, Expr *y, Expr *height )
{
	std::string line( "DRAW " );

	line += std::to_string(dynamic_cast<const Number*>(x)->value);
	line += " ";
	line += std::to_string(dynamic_cast<const Number*>(y)->value);

	if( height ) {
		line += " ";
		line += std::to_string(dynamic_cast<const Number*>(height)->value);
	}

	lines.push_back( line );
}

std::ostream &IRBuilder::operator<<( std::ostream &os ) const
{
	for( auto line : lines ) {
		os << line;
		os << "\n";
	}

	return os;
}
