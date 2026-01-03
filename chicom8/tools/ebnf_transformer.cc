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
    GraphBuilderVisitor graph_visitor( graph );

    grammar.accept( graph_visitor );

    build_connected_component_list();

    std::vector<std::string> forward_decls = most_referenced_nodes( );
    std::vector<std::string> class_list = ordered_class_list( forward_decls );

    grammar_ir = GrammarIR{ forward_decls, class_list };

	return grammar_ir;
}

void Transformer::build_connected_component_list( )
{
    for( auto& [node, node_data] : graph )
        if( ! node_data.visited )
            strong_connect( node );

    for( const auto& [node, node_data] : graph )
        for( const auto& edge : node_data.edges )
            ++graph.at( edge ).usage_count;
}

void Transformer::strong_connect( const Node& node )
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
            strong_connect( neighbour );
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


std::vector<Node> Transformer::most_referenced_nodes( )
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

std::vector<std::string> dag_topo_sort_with_priority( const Graph& graph, const std::vector<std::string>& forward_decls )
{
    // 1️⃣ compute indegrees
    std::unordered_map<std::string, int> indegree;
    for (auto& p : graph) {
        indegree[p.first] = 0; // ensure all nodes exist
    }
    for (auto& p : graph) {
        for (auto& child : p.second.edges) {
            indegree[child]++;
        }
    }

    // 2️⃣ priority function
    auto priority = [&](const std::string& name) -> int {
        auto it = graph.find(name);
        if (it == graph.end()) return 1;       // fallback
        if (it->second.is_base_class) return 0; // base classes first
        if (it->second.edges.empty()) return 2; // leaves last
        return 1;                              // internal nodes
    };

    auto cmp = [&](const std::string& a, const std::string& b) {
        int pa = priority(a), pb = priority(b);
        if (pa != pb) return pa < pb;
        return a < b; // deterministic tiebreaker
    };

    // 3️⃣ initialize zero indegree queue
    std::vector<std::string> zero_indegree;
    for (auto& [node, deg] : indegree)
        if (deg == 0) zero_indegree.push_back(node);

    // sort initially by priority
    std::sort(zero_indegree.begin(), zero_indegree.end(), cmp);

    std::vector<std::string> sorted_list;

    while (!zero_indegree.empty()) {
        // pop first (highest priority)
        std::string node = zero_indegree.front();
        zero_indegree.erase(zero_indegree.begin());

        sorted_list.push_back(node);

        // decrement children indegrees
        for (auto& child : graph.at(node).edges) {
            indegree[child]--;
            if (indegree[child] == 0) {
                zero_indegree.push_back(child);
            }
        }

        // sort again after inserting new zero indegree nodes
        std::sort(zero_indegree.begin(), zero_indegree.end(), cmp);
    }

    // 4️⃣ sanity check: did we include all nodes? (cycle detection)
    if (sorted_list.size() != graph.size()) {
        throw std::runtime_error("Cycle detected in AST graph (should not happen after Tarjan)");
    }

    // 5️⃣ optionally rotate forward-declared nodes to back
    std::unordered_set<std::string> forward_decl_set(forward_decls.begin(), forward_decls.end());
    auto it = std::find_if(sorted_list.begin(), sorted_list.end(),
        [&](const std::string& n){
            return forward_decl_set.contains(n) && !graph.at(n).is_base_class;
        });
    if (it != sorted_list.end())
        std::rotate(sorted_list.begin(), it + 1, sorted_list.end());

    return sorted_list;
}

// std::vector<Node> Transformer::ordered_class_list( const std::vector<std::string>& forward_decls )
// {
//     std::vector<Node> sorted_list;

//     // Step 1: Build node -> SCC index map
//     std::unordered_map<Node, size_t> node_to_scc;
//     for( size_t i = 0; i < sccs.size(); ++i )
//         for( const auto& node : sccs[i] )
//             node_to_scc[node] = i;

//     // Step 2: Collapse multi node cycles into a single node
//     std::unordered_map<size_t, std::unordered_set<size_t>> meta_graph;

//     for( size_t i = 0; i < sccs.size(); ++i )                   // run through all component groups
//         for( const auto& node : sccs[i] )                       // run through all components in this group
//             for( const auto& dep : graph.at(node).edges )       // run through all adjacent nodes
//                 if( node_to_scc[dep] != i )                     // if this adjacent node is not part of the current component group
//                     meta_graph[i].insert( node_to_scc[dep] );   // dependency between different SCCs

//     // Step 3: Topologically sort the meta-graph using DFS
//     std::vector<bool> visited(sccs.size(), false);
//     std::vector<size_t> sorted_scc_ids;

//     std::function<void(size_t)> dfs =
//         [&]( size_t scc_id )
//         {
//             if( visited[scc_id] )
//                 return;

//             visited[scc_id] = true;

//             for( auto dep : meta_graph[scc_id] )
//                 dfs(dep);

//             sorted_scc_ids.push_back( scc_id );
//         };

//     for( size_t i = 0; i < sccs.size(); ++i )
//         dfs(i);

//     std::reverse(sorted_scc_ids.begin(), sorted_scc_ids.end());

//     // Step 4: Order the nodes in a group to put the forward declared node at the back.
//     std::unordered_set<std::string> forward_decl_set( forward_decls.begin(), forward_decls.end() );
//     for( auto& scc : sccs ) {
//         auto it = std::find_if( scc.begin(), scc.end(), [&, forward_decl_set](const Node& node) { return forward_decl_set.contains(node); });
//         if( it != scc.end() )
//             std::rotate(scc.begin(), it + 1, scc.end());
//     }


//     // auto priority = [&]( const std::string& name )
//     // {
//     //     if( graph.at(name).is_base_class ) return 0;
//     //     if( graph.at(name).edges.empty() ) return 2;
//     //     return 1;
//     // };

//     // auto priority_sort = [&](const std::string& a, const std::string& b) { return priority(a) < priority(b); };

//     // // Step 4.5: sort within each SCC by priority
//     // for (auto& scc : sccs)
//     //     std::stable_sort( scc.begin(), scc.end(), priority_sort );

//     // sorted_list = dag_topo_sort_with_priority(graph, forward_decls);
//     // // Step 5: Flatten the SCCs back into sorted node list
//     for( size_t id : sorted_scc_ids )
//         for( const auto& node : sccs[id] )
//             sorted_list.emplace_back(node);


//     return sorted_list;
// }

// Put this method in your Transformer/Generator class implementation file.

std::vector<Node> Transformer::ordered_class_list(const std::vector<std::string>& forward_decls)
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

    // Defensive: ensure we have SCCs and meta order already computed.
    // (This method assumes `sccs` and `sorted_scc_ids` are members computed earlier.)
    if (sccs.empty()) return sorted_list;

    // Build a quick lookup for forward decls
    std::unordered_set<Node> forward_decl_set(forward_decls.begin(), forward_decls.end());

    // Make a local copy of the SCC order produced by your meta-graph DFS.
    // The DFS produced `sorted_scc_ids` in post-order, so reverse it to get topological order.
    std::vector<size_t> scc_order = sorted_scc_ids;
    std::reverse(scc_order.begin(), scc_order.end());

    // Comparator used to sort nodes *inside* an SCC
    auto cmp_in_scc = [&](const Node& a, const Node& b) -> bool
    {
        const auto& ga = graph.at(a);
        const auto& gb = graph.at(b);

        // 1) Base classes first (explicitly set from grammar alternatives)
        if (ga.is_base_class != gb.is_base_class)
            return ga.is_base_class; // true sorts before false

        // 2) If one is the declared inheritance parent of the other, parent first
        //    Note: parent_class holds the name of the parent (empty if none).
        if (!ga.parent_class.empty() && ga.parent_class == b) {
            // b is parent of a → b must come before a
            return false;
        }
        if (!gb.parent_class.empty() && gb.parent_class == a) {
            // a is parent of b → a must come before b
            return true;
        }

        // 3) If both have the same parent, put the parent before children implicitly handled above.
        //    Otherwise, deterministic alphabetical fallback to make output stable.
        return a < b;
    };

    // Process each SCC: sort members by the comparator and then move forward-declared non-base nodes to the end.
    for (size_t scc_id : scc_order) {
        auto& scc = sccs[scc_id];

        // stable sort so relative order among equal items remains deterministic
        std::stable_sort(scc.begin(), scc.end(), cmp_in_scc);

        // Move forward-declared nodes to the end of the SCC, but keep base classes at the front
        // (we never want to push base classes behind forward declarations)
        std::stable_partition(scc.begin(), scc.end(),
            [&](const Node& n) {
                // Keep in front if either:
                //  - not forward-declared, or
                //  - is a base class (must stay early)
                auto it = graph.find(n);
                bool is_base = (it != graph.end()) ? it->second.is_base_class : false;
                bool is_forward = forward_decl_set.contains(n);
                return (!is_forward) || is_base;
            }
        );

        // Append the ordered SCC members to the final list
        for (const auto& node : scc) {
            sorted_list.emplace_back(node);
        }
    }

    // Sanity check (optional): ensure that final order satisfies containment dependencies:
    // every child should appear before its container (i.e. for each edge child→parent, index(child) < index(parent)).
    // If this fails, it's a sign something earlier is wrong (graph direction or parent markers).
#ifdef TRANSFORMER_DEBUG_VALIDATE_ORDER
    {
        std::unordered_map<Node, size_t> index;
        for (size_t i = 0; i < sorted_list.size(); ++i) index[sorted_list[i]] = i;

        for (auto &p : graph) {
            const Node& from = p.first;
            for (const Node& to : p.second.edges) {
                auto itf = index.find(from);
                auto itt = index.find(to);
                if (itf == index.end() || itt == index.end()) continue;
                if (itf->second >= itt->second) {
                    std::cerr << "Ordering violation: '" << from << "' should appear before '" << to << "'\n";
                }
            }
        }
    }
#endif

    return sorted_list;
}


// std::vector<std::string> Transformer::priority_sort( )
// {
//     std::vector<std::string> priority_sorted;

//     auto priority = [&](const std::string& name, const Graph& g) {
//         if (g.at(name).is_base_class) return 0;     // base classes first
//         if (g.at(name).edges.empty()) return 2;     // leaves last
//         return 1;                                   // internal nodes in the middle
//     };

//     for (auto& scc : sccs ) {
//         std::sort(scc.begin(), scc.end(),
//             [&](const std::string& a, const std::string& b){
//                 return priority(a, graph) < priority(b, graph);
//             }
//         );
//     }

//     return priority_sorted;

// }

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
