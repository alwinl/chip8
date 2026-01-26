/*
 * ir_builder.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <string>
#include <vector>
#include <unordered_map>

class Expr;

class IRBuilder
{
public:
	IRBuilder() = default;

	std::string new_label(const std::string& stem);
	void emit_jump_if_false( Expr* expr, std::string label );
	void emit_jump( std::string label );
	void emit_label( std::string label );
	void emit_return();
	void emit_comment( std::string comment );
	void emit_draw( std::string sprite_name, Expr* x, Expr * y, Expr * height );

	std::ostream& operator<<( std::ostream& os ) const;

private:
	std::vector<std::string> lines;
	std::unordered_map<std::string, int> label_map;
};

inline std::ostream& operator<<( std::ostream& os, const IRBuilder& ir ) { return ir.operator<<(os); };
