/*
 * ebnf_transformer.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include "ebnf_graph.h"
#include "ebnf_grammar_ir.h"

using ComponentGroup = std::vector<Node>;
using ComponentGroupList = std::vector<ComponentGroup>;


class Transformer
{
public:
    Transformer( const SyntaxTree& grammar ) : grammar( grammar ) {};

    GrammarIR transform_all();

    std::vector<std::string> get_forward_decls() { return most_referenced_nodes(); }
    std::vector<std::string> get_class_list() { return ordered_class_list( most_referenced_nodes() ); }

    void print_all_cycles( std::ostream& os ) const;
    void print( std::ostream& os ) const;

private:
    SyntaxTree grammar;
    GrammarIR grammar_ir;
    Graph graph;

    ComponentGroupList sccs;
    int index = 0;
    std::stack<Node> node_stack;

    void build_connected_component_list( );
    void strong_connect( const Node& node );

    std::vector<Node> most_referenced_nodes( );
    std::vector<Node> ordered_class_list( const std::vector<std::string>& forward_decls );

    // std::vector<std::string> Transformer::priority_sort( );
};

std::ostream& operator<<( std::ostream& os, const Transformer& transformer );
