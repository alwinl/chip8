/*
 * ebnf_tarjan.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <stack>

using Node = std::string;
using Edge = std::string;
using Edges = std::unordered_set<Edge>;
using Graph = std::unordered_map<Node, Edges>;
using ComponentGroup = std::vector<Node>;
using ComponentGroupList = std::vector<ComponentGroup>;

class TarjanSCC
{
public:
    TarjanSCC( const Graph& graph ) : graph(graph) { run(); }


    std::unordered_set<Node> detect_cycles();
    std::vector<Node> topologicalSort();
    
    void print_all_cycles( std::ostream& os );
    
private:
    const Graph& graph;
    ComponentGroupList sccs;

    std::unordered_map<Node, int> indices;
    std::unordered_map<Node, int> lowlink;
    std::stack<Node> stack;
    std::unordered_set<Node> onStack;
    int index = 0;

    std::unordered_map<Node, int> usage_count;

    void run();
    void strong_connect(const Node& node);
    void get_usage_counts();

};