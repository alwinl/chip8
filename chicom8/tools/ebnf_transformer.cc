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
#include <functional>


GrammarIR Transformer::transform_all()
{
    GraphBuilderVisitor graph_visitor;

    grammar.accept( graph_visitor );

    build_connected_component_list( graph_visitor.graph );

    std::vector<std::string> forward_decls = most_referenced_nodes( graph_visitor.graph, sccs );
    std::vector<std::string> class_list = ordered_class_list( graph_visitor.graph, sccs, forward_decls );

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

std::vector<Node> Transformer::ordered_class_list( Graph &graph, ComponentGroupList& sccs, const std::vector<std::string>& forward_decls )
{
    std::vector<Node> sorted_list;

    // Step 1: Build node -> SCC index map
    std::unordered_map<Node, size_t> node_to_scc;
    for( size_t i = 0; i < sccs.size(); ++i )
        for( const auto& node : sccs[i] )
            node_to_scc[node] = i;

    // Step 2: Collapse multi node cycles into a single node
    std::unordered_map<size_t, std::unordered_set<size_t>> meta_graph;

    for( size_t i = 0; i < sccs.size(); ++i )                   // run through all component groups
        for( const auto& node : sccs[i] )                       // run through all components in this group
            for( const auto& dep : graph.at(node).edges )       // run through all adjacent nodes
                if( node_to_scc[dep] != i )                     // if this adjacent node is not part of the current component group
                    meta_graph[i].insert( node_to_scc[dep] );   // dependency between different SCCs

    // Step 3: Topologically sort the meta-graph using DFS
    std::vector<bool> visited(sccs.size(), false);
    std::vector<size_t> sorted_scc_ids;

    std::function<void(size_t)> dfs =
        [&]( size_t scc_id )
        {
            if( visited[scc_id] )
                return;

            visited[scc_id] = true;

            for( auto dep : meta_graph[scc_id] )
                dfs(dep);

            sorted_scc_ids.push_back( scc_id );
        };

    for( size_t i = 0; i < sccs.size(); ++i )
        dfs(i);

    // Step 4: Order the nodes in a group to put the forward declared node at the back.
    std::unordered_set<std::string> forward_decl_set( forward_decls.begin(), forward_decls.end() );
    for( auto& scc : sccs ) {
        auto it = std::find_if( scc.begin(), scc.end(), [&, forward_decl_set](const Node& node) { return forward_decl_set.contains(node); });
        if( it != scc.end() )
            std::rotate(scc.begin(), it + 1, scc.end());
    }

    // Step 5: Flatten the SCCs back into sorted node list
    for( size_t id : sorted_scc_ids )
        for( const auto& node : sccs[id] )
            sorted_list.emplace_back(node);

    return sorted_list;
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
