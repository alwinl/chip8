/*
 * ebnf_generator.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_generator.h"

#include "ebnf_printer.h"
#include "ebnf_jsonprinter.h"
#include "ebnf_graph.h"
#include "ebnf_transformer.h"

#include <fstream>
#include <stack>
#include <string>

// struct DotVisitor : ASTVisitor
// {
//     DotVisitor( std::ostream& os ) : os(os) {};

//     void visit_symbol( const SymbolNode& symbol )
//     {
//         // if( !rule_stack.empty() && (symbol.token.type == Token::Type::NONTERMINAL) )
//         if( !rule_stack.empty() )
//             os << std::string(indent, ' ') << rule_stack.top() << " -> " << symbol.token.lexeme << ";\n";
//     }

//     void pre_rules( const RuleNode& rule ) { rule_stack.push(rule.name); }
//     void post_rules( const RuleNode& rule ) { rule_stack.pop(); }

//     void pre_syntax( const SyntaxTree& grammar )
//     {
//         os << header;
//         indent += 4;
//     }

//     void post_syntax( const SyntaxTree& grammar )
//     {
//         indent -= 4;
//         os << "}\n";
//     }


//     std::stack<std::string> rule_stack;
//     std::ostream& os;
//     int indent = 0;
//     std::string header =
// R"(
// digraph SyntaxTree
// {
//     // Global layout tuning
//     rankdir=TB;            // Top-to-bottom layout
//     nodesep=0.3;           // Horizontal spacing between nodes
//     ranksep=0.4;           // Vertical spacing between levels
//     splines=true;          // Smooth edges

//     // Node appearance
//     node [
//         shape=box,
//         style="rounded,filled",
//         fillcolor=lightgray,
//         fontname="Consolas",
//         fontsize=10
//     ];

//     // Edge appearance
//     edge [
//         arrowsize=0.7
//     ];

//     // AST edges
// )";
// };

// void Generator::create_dot_source( std::ostream &os )
// {
//     DotVisitor printer( os );
//     syntax_tree.accept( printer );
// }

std::string dot_header =
R"(
digraph SyntaxTree
{
    // Global layout tuning
    rankdir=BT;            // Top-to-bottom layout
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

    // AST edges
)";

std::string dot_footer =
R"(
}
)";

void Generator::create_dot_source( std::ostream &os )
{
    Graph graph;
    GraphBuilderVisitor graph_visitor( graph );
    syntax_tree.accept( graph_visitor );

    os << dot_header;

    for( auto [node, nodedata] : graph )
        for( auto edge : nodedata.edges )
            os << "    " << node << "->" << edge << '\n';

    os << dot_footer;
}

void Generator::create_svg_image( std::string filename )
{
    std::ofstream os( filename + ".dot" );
    if (!os)
        throw std::runtime_error("Cannot open: " + filename);

    create_dot_source( os );

    os.close( );

    std::string cmd = "dot -Tsvg " + filename + ".dot" + " -o " + filename + ".svg";

    if( std::system(cmd.c_str()) != 0 )
        throw std::runtime_error("Graphviz 'dot' command failed.");
}

void Generator::create_grammar_file( std::ostream &os )
{
    JSONPrintVisitor printer( os );
    syntax_tree.accept( printer );
}

void Generator::stream_graph( std::ostream &os )
{
    Graph graph;

    GraphBuilderVisitor graph_visitor( graph );
    syntax_tree.accept( graph_visitor );
    os << graph;
}


std::string convert_to_camel( std::string input )
{
    std::string output;
    output.reserve(input.size());

    bool capitalize_next = true;

    for (char c : input)
    {
        if (c == '_') {
            capitalize_next = true;
        } else {
            if (capitalize_next) {
                output.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
                capitalize_next = false;
            } else {
                output.push_back(c);
            }
        }
    }

    return output;

}

void Generator::create_ast_header( std::ostream& os, std::string tokeniser_header_name )
{
    Transformer transformer( syntax_tree );
    transformer.transform_all();

    std::vector<std::string> forward_decls = transformer.get_forward_decls();
    std::vector<std::string> class_list = transformer.get_class_list();

    // need to figure out which ones are leaves
    Graph graph;

    GraphBuilderVisitor graph_visitor( graph );
    syntax_tree.accept( graph_visitor );

    std::vector<std::string> leaves;
    std::vector<std::string> internal_nodes;

    for( auto& [node, node_data] : graph )
        node_data.edges.empty() ? leaves.emplace_back( node ) : internal_nodes.emplace_back( node );



    // header first
    os <<
R"(
/*
 * AUTOGENERATED FILE. Do not modify.
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

#include <string>
#include <vector>
#include <ostream>
#include <unordered_map>
#include <memory>

)";

    // here we need to put in the tokeniser header file
    os << "#include \"" << tokeniser_header_name << "\"\n";

    os <<
R"(
template <typename T>
std::unique_ptr<T> clone_unique( const std::unique_ptr<T>& src )
{
    return src ? std::unique_ptr<T>(static_cast<T*>(src->clone().release())) : nullptr;
}

template<typename T>
std::vector<T> clone_vector(const std::vector<T>& src)
{
    std::vector<T> result;
    result.reserve(src.size());

    for( const auto& item : src )
        result.push_back( item.clone() );

    return result;
}

template <typename T>
std::vector<std::unique_ptr<T>> clone_vector(const std::vector<std::unique_ptr<T>>& src)
{
    std::vector<std::unique_ptr<T>> result;
    result.reserve(src.size());

    for (const auto& item : src)
        result.push_back(clone_unique(item));

    return result;
}

// forward declarations
struct ASTVisitor;
)";

    for( auto decl : forward_decls )
        os << "struct " << convert_to_camel(decl) << "Node;\n";

    for( auto node : class_list )
        if( graph[node].is_base_class )
            os << "struct " << convert_to_camel(node) << "Node;\n";

    os <<
R"(
// struct definitions
)";
    for( auto node : class_list ) {
        if( graph[node].is_base_class )
            os <<
R"(
/* base class */
)";

        os << "struct " << convert_to_camel(node) << "Node";
        if( !graph[node].parent_class.empty() )
            os << " : " << convert_to_camel( graph[node].parent_class ) << "Node";

        os <<
R"(
{
)";
		os << "/* class implementation */\n";
		os <<
R"(
};


)";
    }

    os <<
R"(

// visitor base class definition
struct ASTVisitor
{
    virtual ~ASTVisitor() = default;

    // leaves
)";
    for( auto leaf : leaves )
        os << "    virtual void visit_" << leaf << "( const " << convert_to_camel(leaf) << "Node& " << leaf << " ) {};\n";

    os <<
R"(
    // internal nodes
)";
    for( auto internal_node : internal_nodes ) {
        os << "    virtual void pre_" << internal_node << "( const " << convert_to_camel(internal_node) << "Node& " << internal_node << " ) {};\n";
        os << "    virtual void post_" << internal_node << "( const " << convert_to_camel(internal_node) << "Node& " << internal_node << " ) {};\n";
    }

    os << R"(
};
)";
}
