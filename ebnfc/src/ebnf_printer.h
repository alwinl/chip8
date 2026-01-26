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
#include <stack>

struct PrintVisitor : ASTVisitor
{
    PrintVisitor( std::ostream& os ) : os(os) {};

    void visit_symbol( const SymbolNode& symbol ) override;
    void visit_literal( const LiteralNode& symbol ) override;

    void pre_group( const GroupNode& group ) override;
    void post_group( const GroupNode& group ) override;
    void pre_elements( const SubPartNode& subpart ) override;
    void post_elements( const SubPartNode& subpart ) override;
    void pre_alternates( const AlternatePartsNode& alternates ) override;
    void post_alternates( const AlternatePartsNode& alternates ) override;
    void pre_production( const ProductionNode& production ) override;
    void post_production( const ProductionNode& production ) override;
    void pre_rules( const RuleNode& rule ) override;
    void post_rules( const RuleNode& rule ) override;
    void pre_syntax( const SyntaxTree& grammar ) override;
    void post_syntax( const SyntaxTree& grammar ) override;

    std::ostream& os;
    int indent = 0;
};

std::ostream& operator<<( std::ostream& os, const SyntaxTree& grammar );
