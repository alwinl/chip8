/*
 * ebnf_printer.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

struct PrintVisitor : ASTVisitor
{
    PrintVisitor( std::ostream& os ) : os(os) {};

    void visit( const Symbol& symbol ) override;
    void pre_symbol( const Symbol& symbol ) override;
    void post_symbol( const Symbol& symbol ) override;
    void pre_group( const Group& group ) override;
    void post_group( const Group& group ) override;
    void pre_elements( const SubPart& subpart ) override;
    void post_elements( const SubPart& subpart ) override;
    void pre_alternates( const AlternateParts& alternates ) override;
    void post_alternates( const AlternateParts& alternates ) override;
    void pre_production( const Rule& rule ) override;
    void post_production( const Rule& rule ) override;
    void pre_rules( const Grammar& grammar ) override;
    void post_rules( const Grammar& grammar ) override;

    std::ostream& os;
    int indent = 0;
};

std::ostream& operator<<( std::ostream& os, const Grammar& grammar );
