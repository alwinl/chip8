/*
 * ebnf_jsonprinter.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_jsonprinter.h"

void print_cardinality( std::ostream& os, const ElementNode::Cardinality& card, int indent )
{
	if( card == ElementNode::Cardinality::ONCE )
		return;

	os << std::string(indent, ' ') << "\"cardinality\": \"";

	switch( card ) {
	case( ElementNode::Cardinality::ONE_OR_MORE ):  os << "+"; break;
	case( ElementNode::Cardinality::OPTIONAL ):     os << "?"; break;
	case( ElementNode::Cardinality::ZERO_OR_MORE ): os << "*"; break;
	default: break;
	}
	os << "\",\n";
}

void JSONPrintVisitor::visit_symbol( const SymbolNode& symbol )
{
	os << std::string(indent, ' ')  << "{\n";
	indent += 4;

	print_cardinality( os, symbol.card, indent );

	os << std::string(indent, ' ')  << "\"Token\" : " << symbol.token << "\n";

	indent -= 4;
	os << std::string(indent, ' ')  << "},";
	os << "\n";
}

void JSONPrintVisitor::visit_literal( const LiteralNode& literal )
{
}

void JSONPrintVisitor::pre_group( const GroupNode& group )
{
	os << std::string(indent, ' ')  << "\"GroupNode\" : {\n";
	indent += 4;

	print_cardinality( os, group.card, indent );
}

void JSONPrintVisitor::post_group( const GroupNode& group )
{
	indent -= 4;
	os << std::string(indent, ' ')  << "}";
	os << "\n";
}

void JSONPrintVisitor::pre_elements( const SubPartNode& subpart )
{
	if( parsing_alternate_parts )
		os << std::string(indent, ' ')  << "[\n";
	else
		os << std::string(indent, ' ')  << "\"SubpartNode\" : [\n";

	indent += 4;
}

void JSONPrintVisitor::post_elements( const SubPartNode& subpart )
{
	indent -= 4;
	os << std::string(indent, ' ')  << "],\n";
}

void JSONPrintVisitor::pre_alternates( const AlternatePartsNode& alternates )
{
	os << std::string(indent, ' ')  << "\"AlternatePartsNode\" : [\n";
	indent += 4;

	parsing_alternate_parts = true;
}

void JSONPrintVisitor::post_alternates( const AlternatePartsNode& alternates )
{
	parsing_alternate_parts = false;

	indent -= 4;
	os << std::string(indent, ' ')  << "]\n";
}

void JSONPrintVisitor::pre_production( const ProductionNode& production )
{
	os << std::string(indent, ' ')  << "\"ProductionNode\" : {\n";
	indent += 4;
}

void JSONPrintVisitor::post_production( const ProductionNode& production )
{
	indent -= 4;
	os << std::string(indent, ' ')  << "}\n";
}

void JSONPrintVisitor::pre_rules( const RuleNode& rule )
{
	os << std::string(indent, ' ')  << "\"RuleNode\" : {\n";
	indent += 4;

	os << std::string(indent, ' ')  << "\"name\" : \"" << rule.name << "\",\n";
}

void JSONPrintVisitor::post_rules( const RuleNode& rule )
{
	indent -= 4;
	os << std::string(indent, ' ')  << "},\n";
}

void JSONPrintVisitor::pre_syntax( const SyntaxTree& grammar )
{
	os << "{\n";
	indent += 4;
	os << std::string(indent, ' ')  << "\"SyntaxTree\" : {\n";
	indent += 4;
}

void JSONPrintVisitor::post_syntax( const SyntaxTree& grammar )
{
	indent -= 4;
	os << "}\n";
	indent -= 4;
	os << "}\n";
}
