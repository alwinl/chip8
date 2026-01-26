/*
 * ebnf_graph.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_graph.h"
#include <set>

std::string header = R"({
	"nodes" : [)";

std::string node_header = R"(
		{
			"name" : ")";

std::string node_name_end = R"(",)";

std::string edge_header = R"(
			"edges" : [
				")";

std::string edge_divider = R"(",
				")";

std::string edge_footer = R"("
			],)";

std::string node_section1 = R"(
			"IsBaseClass" : )";

std::string node_section2 = R"(,
			"ParentClass" : ")";

std::string node_footer = R"("
		})";

std::string footer = R"(
	]
}
)";

std::ostream& operator<<( std::ostream& os, const Graph& graph )
{
	bool first_node = true;

    os << header;

    for( auto& [node, node_data] : graph ) {

		if( first_node )
			first_node = false;
		else
			os << ',';

		bool first = true;

        os << node_header << node << node_name_end;

		if( !node_data.edges.empty() ) {

			os << edge_header;

			for( const Node& edge : node_data.edges ) {
				if( first )
					first = false;
				else
					os << edge_divider;

				os << edge;
			}

			os << edge_footer;
		}

        os << node_section1 << (node_data.is_base_class ? "true" : "false");
        os << node_section2 << node_data.parent_class;
        os << node_footer;
    }

    os << footer;

    return os;
}

struct SemanticProbeVisitor : ASTVisitor
{
    std::optional<std::string> found;
    bool invalid = false;

    void visit_symbol(const SymbolNode&) override;
    void pre_group(const GroupNode&) override;
    void pre_alternates(const AlternatePartsNode&) override;
};

void SemanticProbeVisitor::visit_symbol(const SymbolNode& sym)
{
    if( sym.token.type != Token::Type::NONTERMINAL )
        return;

    if( !found ) {
        found = sym.token.lexeme;
		return;
	}

	if( *found != sym.token.lexeme )
		invalid = true;
}

void SemanticProbeVisitor::pre_group(const GroupNode& group)
{
    if( group.card != ElementNode::Cardinality::ONCE )	// Optional or repeated groups destroy semantic clarity
        invalid = true;
}

void SemanticProbeVisitor::pre_alternates(const AlternatePartsNode&)
{
    invalid = true;	// Nested alternation inside an alternative → not a clean sum type
}


GraphBuilderVisitor::GraphBuilderVisitor( Graph & graph ) : graph(graph)
{
}

void GraphBuilderVisitor::visit_symbol( const SymbolNode& symbol )
{
	// the graph only holds non-terminals
	if( !rule_stack.empty() && (symbol.token.type == Token::Type::NONTERMINAL) )
	{
		auto& lhs = rule_stack.top();

		graph.try_emplace(symbol.token.lexeme);
		graph[symbol.token.lexeme].add_edge( lhs );

		// if( graph[lhs].is_base_class )
		// 	graph[symbol.token.lexeme].parent_class = lhs;
	}
}

void GraphBuilderVisitor::pre_alternates( const AlternatePartsNode& alternates )
{
	std::set<std::string> children;
	auto& lhs = rule_stack.top();

	for( auto& part : alternates.subparts ) {

		SemanticProbeVisitor probe;
		part->accept( probe );

        if( probe.invalid || !probe.found )
            return;

        children.insert( *probe.found );
    }

    if( children.size() != alternates.subparts.size() )
        return;

    graph[lhs].is_base_class = true;

    for( const auto& child : children )
        graph[child].parent_class = lhs;
}

void GraphBuilderVisitor::pre_production( const ProductionNode& production )
{

}

void GraphBuilderVisitor::pre_rules( const RuleNode& rule )
{
	// The name of a rule by definition is non terminal
	rule_stack.push(rule.name);
	graph.try_emplace( rule.name );
}

void GraphBuilderVisitor::post_rules( const RuleNode& rule )
{
	rule_stack.pop();
}
