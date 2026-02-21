 /*
  * cfgemitter.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/cfg_emitter.h"

#include <map>
#include <unordered_map>
#include <array>

static std::array<std::string, opcode_count> mnemonics = {
	"NOP", "CLS", "RET", "JP", "CALL", "SE",
	"SNE", "LD", "ADD", "OR", "AND",
	"XOR", "SUB", "SHR", "SUBN", "SHL", "LD I,",
	"JP V0,", "RND", "DRW", "SKP", "SKNP", "LD DT,",
	"LD ST,", "ST K,", "ST DT,", "ADD I,",
	"LD F,", "LD B,", "ST [I],", "LD [I],"
};

void CFGEmitter::emit( std::ostream &os, const IRProgram &ir, OutputMode mode )
{
	CFGraph graph = build_graph( ir );

	emit_header( os, ir, mode );

	emit_node_header( os, mode );

	for( const auto& node : graph )
		emit_node( os, node.second, mode );

	emit_edge_header( os, mode );

	for( const auto& node : graph )
		emit_edge( os, node.second, mode );

	emit_footer( os, ir, mode );
}

CFGEmitter::CFGraph CFGEmitter::build_graph( const IRProgram &ir )
{
	CFGraph graph;

	for (const auto& element : ir.elements) {

		if( auto* instruction_element = std::get_if<InstructionElement>( &element ) ) {

			CFGNode node;

			node.address = instruction_element->address;
			node.mnemonic =  mnemonics[ static_cast<int>(instruction_element->instruction.opcode()) ];

			switch( instruction_element->instruction.opcode() ) {
			case Opcode::CALL:
				node.successors.push_back( { static_cast<uint16_t>(instruction_element->address + 2), "" } );
				// fall through

			case Opcode::JP: {
				auto target = std::get<Addr>(instruction_element->instruction.operands()[0]).value;
				node.successors.push_back( { target, "" } );
				break;
			}

			case Opcode::RET:
				break;

			case Opcode::SE_Imm:
			case Opcode::SE_Reg:
			case Opcode::SKP:
				node.successors.push_back( { static_cast<uint16_t>(instruction_element->address + 2), "false" } );
				node.successors.push_back( { static_cast<uint16_t>(instruction_element->address + 4), "true" } );
				break;

			case Opcode::SNE_Imm:
			case Opcode::SNE_Reg:
			case Opcode::SKNP:
				node.successors.push_back( { static_cast<uint16_t>(instruction_element->address + 2), "false" } );
				node.successors.push_back( { static_cast<uint16_t>(instruction_element->address + 4), "true" } );
				break;

			default:
				node.successors.push_back( { static_cast<uint16_t>(instruction_element->address + 2), "" } );
				break;
			}

			graph[ instruction_element->address ] = std::move(node);
		}
	}

	return graph;
}

void CFGEmitter::emit_header( std::ostream &os, const IRProgram &ir, OutputMode mode )
{
	if( mode == OutputMode::PlantUML ) {
		os << "@startuml cfg\ntitle Instruction-level CFG\n";
	} else {
		os << R"(digraph CFG
{
    // Global layout tuning
    //rankdir=BT;            // Top-to-bottom layout
    nodesep=0.3;           // Horizontal spacing between nodes
    ranksep=0.4;           // Vertical spacing between levels
    splines=true;          // Smooth edges

    // Node appearance
    node [
        shape=box,
        style="rounded,filled",
        fillcolor=lightgray,
        fontname="Consolas",
        fontsize=10
    ];

    // Edge appearance
    edge [
        arrowsize=0.7
    ];
)";
	}
}

void CFGEmitter::emit_node_header( std::ostream &os, OutputMode mode )
{
	if( mode == OutputMode::PlantUML )
		os << "\n    ' Nodes\n";
	else
		os << "\n    // Nodes\n";
}

void CFGEmitter::emit_node( std::ostream &os, const CFGNode& node, OutputMode mode )
{
	if( mode == OutputMode::PlantUML )
		os << "    node \"0x" << std::hex << node.address << "\\n" << node.mnemonic << "\" as n" << node.address << "\n";
	else
		os << "    n" << std::hex << node.address << " [label=\"0x" << std::hex << node.address << ": " << node.mnemonic << "\"];\n";
}

void CFGEmitter::emit_edge_header( std::ostream &os, OutputMode mode )
{
	if( mode == OutputMode::PlantUML )
		os << "\n    ' Edges\n";
	else
		os << "\n    // Edges\n";
}

void CFGEmitter::emit_edge( std::ostream &os, const CFGNode &node, OutputMode mode )
{
	if( mode == OutputMode::PlantUML) {
		for( auto succ : node.successors ) {
			os << "    n" << std::hex << node.address
			<< " --> n" << std::hex << succ.address;
			if( !succ.condition.empty() )
				os << " : " << succ.condition;
			os << "\n";
		}
	} else {
		for( auto succ : node.successors ) {
			os << "   n" << std::hex << node.address
			<< " -> n" << std::hex << succ.address;
			if( !succ.condition.empty() )
				os << " [ label=" << succ.condition << " ]";
			os << ";\n";
		}
	}
}

void CFGEmitter::emit_footer( std::ostream &os, const IRProgram &ir, OutputMode mode )
{
	if( mode == OutputMode::PlantUML )
		os << "\n@enduml\n";
	else
		os << "\n}\n";
}
