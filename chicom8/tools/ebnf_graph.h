/*
 * ebnf_graph.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_ast.h"

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <stack>

using Node = std::string;
using Edges = std::unordered_set<Node>;

struct NodeData
{
    void add_edge( std::string other_node ) { edges.insert( other_node ); }

    Edges edges;
    bool visited = false;
    bool on_stack = false;
    int index = -1;
    int lowlink = -1;
    int usage_count = 0;

    bool is_base_class = false;
    std::string parent_class;
};

using Graph = std::unordered_map<Node, NodeData>;


struct GraphBuilderVisitor : ASTVisitor
{
    GraphBuilderVisitor( Graph & graph ) : graph(graph) {};

    void visit_symbol( const SymbolNode& symbol ) override
    {
        // the graph only holds non-terminals
        if( !rule_stack.empty() && (symbol.token.type == Token::Type::NONTERMINAL) )
        {
            auto& lhs = rule_stack.top();
            // graph[lhs].add_edge( symbol.token.lexeme );
            // graph.try_emplace(symbol.token.lexeme);
            graph.try_emplace(symbol.token.lexeme);
            graph[symbol.token.lexeme].add_edge( lhs );
            if( graph[lhs].is_base_class )
                graph[symbol.token.lexeme].parent_class = lhs;
        }
    }

    void pre_alternates( const AlternatePartsNode& alternates )
    {
        auto& lhs = rule_stack.top();
        graph[lhs].is_base_class = true;
    };

    void pre_rules( const RuleNode& rule ) override
    {
        // The name of a rule by definition is non terminal
        rule_stack.push(rule.name);
        graph.try_emplace( rule.name );
    }

    void post_rules( const RuleNode& rule ) override
    {
        rule_stack.pop();
    }

    Graph & graph;
    std::stack<std::string> rule_stack;
};

std::ostream& operator<<( std::ostream& os, const Graph& graph );
