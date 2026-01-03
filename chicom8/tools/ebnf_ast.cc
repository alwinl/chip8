/*
 * ebnf_ast.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <memory>

#include "ebnf_ast.h"

void SymbolNode::accept( ASTVisitor& visitor )
{
    visitor.visit_symbol( *this);
}

void LiteralNode::accept( ASTVisitor& visitor )
{
    visitor.visit_literal( *this);
}

void GroupNode::accept( ASTVisitor& visitor )
{
    visitor.pre_group( *this );

    if( inner )
        inner->accept( visitor );

    visitor.post_group( *this );
}

void SubPartNode::accept( ASTVisitor& visitor )
{
    visitor.pre_elements( *this );

    for( const auto& e : element )
        e->accept( visitor );

    visitor.post_elements( *this );
}

void AlternatePartsNode::accept( ASTVisitor& visitor )
{
    visitor.pre_alternates( *this );

    for( const auto& part : subparts )
        part->accept( visitor );

    visitor.post_alternates( *this );
}

void ProductionNode::accept( ASTVisitor& visitor )
{
    visitor.pre_production( *this );

    content->accept( visitor );

    visitor.post_production( *this );
}

void RuleNode::accept( ASTVisitor& visitor ) const
{
    visitor.pre_rules( *this );

    production->accept( visitor );

    visitor.post_rules( *this );
}

void SyntaxTree::accept( ASTVisitor& visitor ) const
{
    visitor.pre_syntax( *this );

    for( auto& rule : rules )
        rule.accept( visitor );

    visitor.post_syntax( *this );
}


bool equals( const PartNode& a, const PartNode& b );
bool equals( const ElementNode& a, const ElementNode& b );
bool equals( const ProductionNode& a, const ProductionNode& b );

template <typename T>
inline bool equals(const std::unique_ptr<T>& a, const std::unique_ptr<T>& b)
{
    if (!a && !b) return true;
    if (!a || !b) return false;

    return equals(static_cast<const T&>(*a), static_cast<const T&>(*b));
}

template <typename T>
bool equals_vector( const std::vector<T>& a, const std::vector<T>& b )
{
    if (a.size() != b.size())
        return false;

    for( size_t i = 0; i < a.size(); ++i )
        if( !equals( a[i], b[i] ) )
            return false;

    return true;
}

template <typename T>
bool equals_vector( const std::vector<std::unique_ptr<T>>& a, const std::vector<std::unique_ptr<T>>& b )
{
    if (a.size() != b.size())
        return false;

    for( size_t i = 0; i < a.size(); ++i )
        if( !equals( a[i], b[i] ) )
            return false;

    return true;
}

bool equals( const ElementNode& a, const ElementNode& b )
{
    if( typeid(a) != typeid(b) )
        return false;

    if( auto sa = dynamic_cast<const SymbolNode*>(&a) )
        return (sa->token == dynamic_cast<const SymbolNode*>(&b)->token ) &&
                (sa->card == dynamic_cast<const SymbolNode*>(&b)->card );

    if( auto aa = dynamic_cast<const GroupNode*>(&a) )
        return equals( aa->inner, dynamic_cast<const GroupNode*>(&b)->inner );

    return false;
}

bool equals( const PartNode& a, const PartNode& b )
{
    if( typeid(a) != typeid(b) )
        return false;

    if( auto sa = dynamic_cast<const SubPartNode*>(&a) )
        return equals_vector( sa->element, dynamic_cast<const SubPartNode*>(&b)->element );

    if( auto aa = dynamic_cast<const AlternatePartsNode*>(&a) )
        return equals_vector( aa->subparts, dynamic_cast<const AlternatePartsNode*>(&b)->subparts );

    return false;
}

bool equals( const ProductionNode& a, const ProductionNode& b )
{
    return equals( a.content, b.content );
}

bool equals( const RuleNode& a, const RuleNode& b )
{
    return( a.name == b.name && equals( a.production, b.production) );
}

bool equals( const SyntaxTree& a, const SyntaxTree& b )
{
    if (a.rules.size() != b.rules.size())
        return false;

    for( size_t i = 0; i < a.rules.size(); ++i )
        if( ! equals( a.rules[i], b.rules[i]) )
            return false;

    return true;
}

bool operator==( const SyntaxTree& a, const SyntaxTree& b )
{
    return equals( a, b );
}
