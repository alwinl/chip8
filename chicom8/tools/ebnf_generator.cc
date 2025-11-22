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
#include "ebnf_graph.h"

#include <fstream>
#include <stack>
#include <string>

struct DotVisitor : ASTVisitor
{
    DotVisitor( std::ostream& os ) : os(os) {};

    void visit( const SymbolNode& symbol )
    {
        if( !rule_stack.empty() && (symbol.token.type == Token::Type::NONTERMINAL) )
            os << std::string(indent, ' ') << rule_stack.top() << " -> " << symbol.token.lexeme << ";\n";
    }

    void pre_rules( const RuleNode& rule ) { rule_stack.push(rule.name); }
    void post_rules( const RuleNode& rule ) { rule_stack.pop(); }

    void pre_syntax( const SyntaxTree& grammar )
    {
        os << header;
        indent += 4;
    }

    void post_syntax( const SyntaxTree& grammar )
    {
        indent -= 4;
        os << "}\n";
    }


    std::stack<std::string> rule_stack;
    std::ostream& os;
    int indent = 0;
    std::string header =
R"(
digraph SyntaxTree
{
    // Global layout tuning
    rankdir=TB;            // Top-to-bottom layout
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
};

void Generator::create_dot_source( std::ostream &os )
{
    DotVisitor printer( os );
    syntax_tree.accept( printer );
}

void Generator::create_svg_image( std::string filename )
{
    std::ofstream os( filename + ".dot" );

    if (!os)
        throw std::runtime_error("Cannot open: " + filename);

    DotVisitor printer( os );
    syntax_tree.accept( printer );

    os.flush();
    os.close();

    std::string cmd = "dot -Tsvg " + filename + ".dot" + " -o " + filename + ".svg";

    if( std::system(cmd.c_str()) != 0 )
        throw std::runtime_error("Graphviz 'dot' command failed.");
}

void Generator::create_grammar_file( std::string filename )
{
    std::ofstream os( filename );
    if( !os )
        throw std::runtime_error("Cannot open: " + filename);

    PrintVisitor printer( os );
    syntax_tree.accept( printer );

}

void Generator::stream_graph( std::ostream &os )
{
    GraphBuilderVisitor graph_visitor;
    syntax_tree.accept( graph_visitor );
    os << graph_visitor.graph;
}
