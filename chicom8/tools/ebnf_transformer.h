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

    void print_all_cycles( std::ostream& os ) const;
    void print( std::ostream& os ) const;

    void create_dot_source( std::ostream& os, const SyntaxTree& grammar );
    void create_svg_image( std::string filename, const SyntaxTree& grammar );


private:
    SyntaxTree grammar;
    GrammarIR grammar_ir;

    ComponentGroupList sccs;
    int index = 0;
    std::stack<Node> node_stack;

    void build_connected_component_list( Graph& graph );
    void strong_connect( Graph &graph, const Node& node );

    std::vector<Node> most_referenced_nodes( Graph &graph, ComponentGroupList& sccs );
    std::vector<Node> ordered_class_list( Graph &graph, ComponentGroupList& sccs, const std::vector<std::string>& forward_decls );
};

std::ostream& operator<<( std::ostream& os, const Transformer& transformer );
