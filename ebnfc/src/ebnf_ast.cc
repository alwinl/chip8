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

    for( const auto& rule : rules )
        rule.accept( visitor );

    visitor.post_syntax( *this );
}


bool equals( const PartNode& lhs, const PartNode& rhs );
bool equals( const ElementNode& lhs, const ElementNode& rhs );
bool equals( const ProductionNode& lhs, const ProductionNode& rhs );

template <typename T>
inline bool equals(const std::unique_ptr<T>& lhs, const std::unique_ptr<T>& rhs)
{
    if (!lhs && !rhs) return true;
    if (!lhs || !rhs) return false;

    return equals(static_cast<const T&>(*lhs), static_cast<const T&>(*rhs));
}

template <typename T>
bool equals_vector( const std::vector<T>& lhs, const std::vector<T>& rhs )
{
    if (lhs.size() != rhs.size())
        return false;

    for( size_t i = 0; i < lhs.size(); ++i )
        if( !equals( lhs[i], rhs[i] ) )
            return false;

    return true;
}

template <typename T>
bool equals_vector( const std::vector<std::unique_ptr<T>>& lhs, const std::vector<std::unique_ptr<T>>& rhs )
{
    if (lhs.size() != rhs.size())
        return false;

    for( size_t i = 0; i < lhs.size(); ++i )
        if( !equals( lhs[i], rhs[i] ) )
            return false;

    return true;
}

bool equals( const ElementNode& lhs, const ElementNode& rhs )
{
    if( typeid(lhs) != typeid(rhs) )
        return false;

    if( const auto * lhs_symbol = dynamic_cast<const SymbolNode*>(&lhs) )
        return (lhs_symbol->token == dynamic_cast<const SymbolNode*>(&rhs)->token ) &&
                (lhs_symbol->card == dynamic_cast<const SymbolNode*>(&rhs)->card );

    if( const auto * lhs_group = dynamic_cast<const GroupNode*>(&lhs) )
        return equals( lhs_group->inner, dynamic_cast<const GroupNode*>(&rhs)->inner );

    return false;
}

bool equals( const PartNode& lhs, const PartNode& rhs )
{
    if( typeid(lhs) != typeid(rhs) )
        return false;

    if( const auto * lhs_subpart = dynamic_cast<const SubPartNode*>(&lhs) )
        return equals_vector( lhs_subpart->element, dynamic_cast<const SubPartNode*>(&rhs)->element );

    if( const auto * lhs_altpart = dynamic_cast<const AlternatePartsNode*>(&lhs) )
        return equals_vector( lhs_altpart->subparts, dynamic_cast<const AlternatePartsNode*>(&rhs)->subparts );

    return false;
}

bool equals( const ProductionNode& lhs, const ProductionNode& rhs )
{
    return equals( lhs.content, rhs.content );
}

bool equals( const RuleNode& lhs, const RuleNode& rhs )
{
    return( lhs.name == rhs.name && equals( lhs.production, rhs.production) );
}

bool equals( const SyntaxTree& lhs, const SyntaxTree& rhs )
{
    if (lhs.rules.size() != rhs.rules.size())
        return false;

    for( size_t i = 0; i < lhs.rules.size(); ++i )
        if( ! equals( lhs.rules[i], rhs.rules[i]) )
            return false;

    return true;
}

bool operator==( const SyntaxTree& lhs, const SyntaxTree& rhs )
{
    return equals( lhs, rhs );
}
