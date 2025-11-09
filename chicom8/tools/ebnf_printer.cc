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

std::ostream& operator<<( std::ostream& os, const Cardinality& card )
{
    if( card == Cardinality::ONE_OR_MORE ) os << "+";
    if( card == Cardinality::OPTIONAL ) os << "?";
    if( card == Cardinality::ZERO_OR_MORE ) os << "*";

    return os;
}

void PrintVisitor::visit( const Symbol& symbol )
{
    os << symbol.token << symbol.card;
}

void PrintVisitor::pre_symbol( const Symbol& symbol )
{
    os << std::string(indent, ' ')  << "Symbol "<< ": ";
    indent += 4;
}

void PrintVisitor::post_symbol( const Symbol& symbol )
{
    indent -= 4;
    os << "\n";
}

void PrintVisitor::pre_group( const Group& group )
{
    os << std::string(indent, ' ')  << "Group "<< ": {\n";
    indent += 4;
}

void PrintVisitor::post_group( const Group& group )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}" << group.card;

    os << "\n";
}

void PrintVisitor::pre_elements( const SubPart& subpart )
{
    os << std::string(indent, ' ')  << "Subpart "<< ": {\n";
    indent += 4;
}

void PrintVisitor::post_elements( const SubPart& subpart )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::pre_alternates( const AlternateParts& alternates )
{
    os << std::string(indent, ' ')  << "Alternates "<< ": {\n";
    indent += 4;
}

void PrintVisitor::post_alternates( const AlternateParts& alternates )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::pre_production( const Rule& rule )
{
    os << std::string(indent, ' ')  << "Rule " << rule.name << ": {\n";
    indent += 4;
}

void PrintVisitor::post_production( const Rule& rule )
{
    indent -= 4;
    os << std::string(indent, ' ')  << "}\n";
}

void PrintVisitor::pre_rules( const Grammar& grammar )
{
    os << "Grammar: {\n";
    indent += 4;
}

void PrintVisitor::post_rules( const Grammar& grammar )
{
    indent -= 4;
    os << "}\n";
}


std::ostream& operator<<( std::ostream& os, const Grammar& grammar )
{
    PrintVisitor printer( os );
    grammar.accept( printer );
    return os;
}
