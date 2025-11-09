/*
 * ebnf_tarjan.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_tarjan.h"
#include <algorithm>
#include <functional>

void TarjanSCC::run()
{
    for( const auto& [node, _] : graph )
        if( indices.find(node) == indices.end() )
            strong_connect( node );

    get_usage_counts();
}

void TarjanSCC::strong_connect( const Node& node )
{
    indices[node] = lowlink[node] = index++;
    stack.push(node);
    onStack.insert(node);

    const auto it = graph.find(node);
    if( it != graph.end() ) {

        for( const Edge& neighbour : it->second ) {

            if( indices.find(neighbour) == indices.end() ) {
                // Neighbour has not yet been visited; recurse on it
                strong_connect( neighbour );
                lowlink[node] = std::min( lowlink[node], lowlink[neighbour] );
            } else if( onStack.count(neighbour) ) {
                // The neighbour is on stack, thus belongs to the current SCC
                lowlink[node] = std::min( lowlink[node], indices[neighbour] );
            }
            // If the neighbour has been visited but not on the stack, then the neighbour belongs to a different SCC and must be ignored
        }
    }

    // If node is a root node, pop the stack and generate an SCC
    if( lowlink[node] == indices[node] ) {

        ComponentGroup new_component;
        Node member_node;

        do {

            member_node = stack.top();
            stack.pop();
            onStack.erase(member_node);

            new_component.push_back(member_node);

        } while( member_node != node );

        sccs.push_back( std::move(new_component) );
    }
}

void TarjanSCC::get_usage_counts()
{
    for( const auto& [node, edges] : graph )
        for( const auto& edge : edges )
            ++usage_count[edge];
}

void TarjanSCC::print_all_cycles( std::ostream& os )
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
            
        os << std::string("\n");
    }
}

std::unordered_set<Node> TarjanSCC::detect_cycles()
{
    std::unordered_set<Node> forward_declarations;

    auto comp_func = [&]( const Node& a, const Node& b ) { return usage_count.at(a) < usage_count.at(b); };

    for( const ComponentGroup& cycle : sccs )
        if( cycle.size() > 1 ) {
            auto it = std::max_element( cycle.begin(), cycle.end(), comp_func );
            forward_declarations.insert( *it );
        }

    return forward_declarations;
}

void rotate_scc( std::vector<Node>& scc, const std::unordered_set<Node>& forward_decls )
{
    // auto it = std::find_if( scc.begin(), scc.end(), [&](const Node& node) { return forward_decls.contains(node); });

    // if( it != scc.end() )
    //     std::rotate(scc.begin(), it + 1, scc.end());
}

std::vector<Node> TarjanSCC::topologicalSort()
{
    std::vector<Node> sorted;

    // Step 1: Build node -> SCC index map
    std::unordered_map<Node, size_t> node_to_scc;
    for( size_t i = 0; i < sccs.size(); ++i )
        for( const auto& node : sccs[i] )
            node_to_scc[node] = i;

    // Step 2: Build a meta-graph of SCC dependencies
    std::unordered_map<size_t, std::unordered_set<size_t>> meta_graph;

    // I believe this collapses a cycle into a single node
    for( size_t i = 0; i < sccs.size(); ++i ) {
        for( const auto& node : sccs[i] ) {
            auto it = graph.find(node);
            if( it != graph.end() ) {
                for( const auto& dep : it->second ) {
                    size_t dep_scc = node_to_scc[dep];
                    if( dep_scc != i ) {
                        meta_graph[i].insert(dep_scc);  // dependency between different SCCs
                    }
                }
            }
        }
    }

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

    // std::reverse( sorted_scc_ids.begin(), sorted_scc_ids.end() );

    // Step 4: Order the nodes in a group to put the forward declared node at the back.
    std::unordered_set<Node> forward_decls = detect_cycles();
    for( auto& scc : sccs )
        rotate_scc( scc, forward_decls );

    // Step 5: Flatten the SCCs back into sorted node list
    for( size_t id : sorted_scc_ids )
        for( const auto& node : sccs[id] )
            sorted.emplace_back(node);

    return sorted;
}


