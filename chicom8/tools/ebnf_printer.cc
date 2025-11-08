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

void PrintVisitor::visit( const Group& group )
{
    os << std::string(indent, ' ')  << "Group "<< ": {\n";
    indent += 4;

    visit( *group.inner );

    indent -= 4;
    os << std::string(indent, ' ')  << "}";
    if( group.card == Cardinality::ONE_OR_MORE ) os << "+";
    if( group.card == Cardinality::OPTIONAL ) os << "?";
    if( group.card == Cardinality::ZERO_OR_MORE ) os << "*";
    os << "\n";
}

void PrintVisitor::visit( const Symbol& symbol )
{
    os << std::string(indent, ' ')  << "Symbol "<< ": ";
    indent += 4;

    os << symbol.token;
    if( symbol.card == Cardinality::ONE_OR_MORE ) os << "+";
    if( symbol.card == Cardinality::OPTIONAL ) os << "?";
    if( symbol.card == Cardinality::ZERO_OR_MORE ) os << "*";

    indent -= 4;
    os << "\n";
}

void PrintVisitor::visit( const SubPart& subpart )
{
    os << std::string(indent, ' ')  << "Subpart "<< ": {\n";
    indent += 4;

    for( const auto& element : subpart.element )
        element->accept( *this );        // dynamic dispatch

    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::visit( const AlternateParts& alternates )
{
    os << std::string(indent, ' ')  << "Alternates "<< ": {\n";
    indent += 4;

    for( const auto& part : alternates.subparts )
        part->accept( *this );        // dynamic dispatch

    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::visit( const Production& production )
{
    // os << std::string(indent, ' ')  << "Production "<< ": {\n";
    // indent += 4;

    production.content->accept( *this );        // dynamic dispatch

    // indent -= 4;
    // os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::visit( const Rule& rule )
{
    os << std::string(indent, ' ')  << "Rule " << rule.name << ": {\n";
    indent += 4;

    visit( *rule.production );

    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::visit( const Grammar& grammar )
{
    os << "Grammar: {\n";
    indent += 4;

    for( const auto& rule : grammar.rules )
        visit( rule );

    indent -= 4;
    os << "}\n";
}

std::ostream& operator<<( std::ostream& os, const Grammar& grammar )
{
    PrintVisitor printer( os );
    printer.visit( grammar );

    return os;
}

