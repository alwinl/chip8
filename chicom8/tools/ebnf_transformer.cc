/*
 * ebnf_transformer.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <algorithm>

#include "ebnf_transformer.h"
#include "ebnf_graph.h"


GrammarIR Transformer::transform_all()
{
    GraphBuilderVisitor graph_visitor;

    grammar.accept( graph_visitor );

    build_connected_component_list( graph_visitor.graph );

    std::vector<std::string> forward_decls = most_referenced_nodes( graph_visitor.graph, sccs );
    std::vector<std::string> class_list = ordered_class_list( sccs );

    grammar_ir = GrammarIR{ forward_decls, class_list };

	return grammar_ir;
}

void Transformer::build_connected_component_list( Graph &graph )
{
    for( auto& [node, node_data] : graph )
        if( ! node_data.visited )
            strong_connect( graph, node );

    for( const auto& [node, node_data] : graph )
        for( const auto& edge : node_data.edges )
            ++graph.at( edge ).usage_count;
}

void Transformer::strong_connect( Graph &graph, const Node& node )
{
    NodeData& node_data = graph.at( node );

    node_data.lowlink =
    node_data.index = index++;
    node_data.visited = true;

    node_stack.push( node );
    node_data.on_stack = true;

    for( const auto& neighbour : node_data.edges ) {

        NodeData& next = graph.at( neighbour );

        if( !next.visited ) {
            strong_connect( graph, neighbour );
            node_data.lowlink = std::min( node_data.lowlink, next.lowlink );
        } else if( next.on_stack )
            node_data.lowlink = std::min( node_data.lowlink, next.index );
    }

    if( node_data.lowlink == node_data.index ) {

        ComponentGroup new_component;
        Node member_node;

        do {
            member_node = node_stack.top();
            node_stack.pop();
            graph.at( member_node ).on_stack = false;

            new_component.push_back( member_node );

        } while( member_node != node );

        sccs.push_back( std::move( new_component) );
    }
}


std::vector<Node> Transformer::most_referenced_nodes( Graph &graph, ComponentGroupList& sccs )
{
    std::vector<Node> forward_declarations;

    auto comp_func = [&]( const Node& a, const Node& b ) { return graph.at(a).usage_count < graph.at(b).usage_count; };

    for( const ComponentGroup& cycle : sccs )
        if( cycle.size() > 1 ) {
            auto it = std::max_element( cycle.begin(), cycle.end(), comp_func );
            forward_declarations.emplace_back( *it );
        }

    return forward_declarations;
}

std::vector<std::string> Transformer::ordered_class_list( ComponentGroupList& sccs )
{
	return std::vector<std::string>();
}


void Transformer::print_all_cycles( std::ostream& os ) const
{
    for( const auto& cycle : sccs ) {

        bool first = true;
        for( const auto& node : cycle ) {
            if( first )
                first = false;
            else
                os << std::string(" --> ");

            os << node;
        }

        if (cycle.size() > 1)
            os << "  [recursive cycle]";

        os << std::string("\n");
    }
}



void Transformer::print( std::ostream &os ) const
{
    os << "Forward declarations:\n";
    for( const auto& fwd : grammar_ir.forward_decls )
        os << "  " << fwd << '\n';

    os << "\nClass list:\n";
    for( const auto& cls : grammar_ir.class_list )
        os << "  " << cls << '\n';
}

std::ostream& operator<<( std::ostream& os, const Transformer& transformer )
{
    transformer.print( os );
	return os;
};
