/*
 * cfgemitter.h Copyright 2026 Alwin Leerling <dna.leerling@gmail.com>
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

#include <ostream>
#include <cstdint>
#include <vector>
#include <map>
#include <string>

#include "ir/chip8ir.h"

class CFGEmitter
{
public:
	enum class OutputMode { PlantUML, Dot };

	void emit( std::ostream& os, const IRProgram& ir, OutputMode mode );

private:
	struct CFGNode {
		uint16_t address;
		std::string mnemonic;
		std::vector<uint16_t> successors;
	};

	using CFGraph = std::map<uint16_t, CFGNode>;

	CFGraph build_graph( const IRProgram &ir );
	void emit_header( std::ostream &os, const IRProgram &ir, OutputMode mode );
	void emit_node_header( std::ostream &os, OutputMode mode );
	void emit_node( std::ostream &os, const CFGNode& node, OutputMode mode );
	void emit_edge_header( std::ostream &os, OutputMode mode );
	void emit_edge( std::ostream &os, const CFGNode& node, OutputMode mode );
	void emit_footer( std::ostream &os, const IRProgram &ir, OutputMode mode );
};
