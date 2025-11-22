/*
 * ebnf_printer.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_printer.h"

#include <fstream>

std::ostream& operator<<( std::ostream& os, const ElementNode::Cardinality& card )
{
    if( card == ElementNode::Cardinality::ONE_OR_MORE ) os << "+";
    if( card == ElementNode::Cardinality::OPTIONAL ) os << "?";
    if( card == ElementNode::Cardinality::ZERO_OR_MORE ) os << "*";

    return os;
}

void PrintVisitor::visit( const SymbolNode& symbol )
{
    os << symbol.token << symbol.card;
}

void PrintVisitor::pre_symbol( const SymbolNode& symbol )
{
    os << std::string(indent, ' ')  << "SymbolNode "<< ": ";
    indent += 4;
}

void PrintVisitor::post_symbol( const SymbolNode& symbol )
{
    indent -= 4;
    os << "\n";
}

void PrintVisitor::pre_group( const GroupNode& group )
{
    os << std::string(indent, ' ')  << "GroupNode "<< ": {\n";
    indent += 4;
}

void PrintVisitor::post_group( const GroupNode& group )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}" << group.card;

    os << "\n";
}

void PrintVisitor::pre_elements( const SubPartNode& subpart )
{
    os << std::string(indent, ' ')  << "SubpartNode "<< ": {\n";
    indent += 4;
}

void PrintVisitor::post_elements( const SubPartNode& subpart )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::pre_alternates( const AlternatePartsNode& alternates )
{
    os << std::string(indent, ' ')  << "AlternatePartsNode "<< ": {\n";
    indent += 4;
}

void PrintVisitor::post_alternates( const AlternatePartsNode& alternates )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::pre_production( const RuleNode& rule )
{
    os << std::string(indent, ' ')  << "RuleNode " << rule.name << ": {\n";
    indent += 4;
}

void PrintVisitor::post_production( const RuleNode& rule )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::pre_rules( const SyntaxTree& grammar )
{
    os << "SyntaxTree: {\n";
    indent += 4;
}

void PrintVisitor::post_rules( const SyntaxTree& grammar )
{
    indent -= 4;
    os << "}\n";
}


std::ostream& operator<<( std::ostream& os, const SyntaxTree& grammar )
{
    PrintVisitor printer( os );
    grammar.accept( printer );
    return os;
}

void generate_svg(const std::string& dotFile, const std::string& svgFile)
{
    std::string cmd = "dot -Tsvg " + dotFile + " -o " + svgFile;
    int result = std::system(cmd.c_str());
    if (result != 0) {
        throw std::runtime_error("Graphviz 'dot' command failed.");
    }
}

void create_dot_source( std::string filename, const SyntaxTree& grammar )
{
    std::ofstream os( filename + ".dot" );

    if (!os)
        throw std::runtime_error("Cannot open: " + filename);


    DotVisitor printer( os );
    grammar.accept( printer );

    os.flush();
    os.close();

    generate_svg( filename + ".dot",  filename + ".svg");
}

void DotVisitor::pre_rules( const SyntaxTree &grammar )
{
    os << "digraph SyntaxTree\n{";
    indent += 4;

    std::string header =
R"(
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
    os << header;
}

void DotVisitor::post_rules( const SyntaxTree &grammar )
{
    indent -= 4;
    os << "}\n";
}

void DotVisitor::visit( const SymbolNode& symbol )
{
    if( !rule_stack.empty() && (symbol.token.type == Token::Type::NONTERMINAL) )
    {
        auto& lhs = rule_stack.top();
        // graph[lhs].add_edge( symbol.token.lexeme );
        // graph.try_emplace(symbol.token.lexeme);
        os << std::string(indent, ' ') << lhs << " -> " << symbol.token.lexeme << ";\n";
    }
}

void DotVisitor::pre_production( const RuleNode& rule )
{
    rule_stack.push(rule.name);
    // graph.try_emplace( rule.name );
    // os << rule.name << ";\n";
}

void DotVisitor::post_production( const RuleNode& rule )
{
    rule_stack.pop();
}
