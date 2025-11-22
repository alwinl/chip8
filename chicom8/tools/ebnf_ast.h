/*
 * ebnf_ast.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <string>
#include <vector>
#include <ostream>
#include <unordered_map>
#include <memory>

#include "ebnf_token.h"

template <typename T>
std::unique_ptr<T> clone_unique( const std::unique_ptr<T>& src )
{
    return src ? std::unique_ptr<T>(static_cast<T*>(src->clone().release())) : nullptr;
}

template<typename T>
std::vector<T> clone_vector(const std::vector<T>& src)
{
    std::vector<T> result;
    result.reserve(src.size());

    for( const auto& item : src )
        result.push_back( item.clone() );

    return result;
}

template <typename T>
std::vector<std::unique_ptr<T>> clone_vector(const std::vector<std::unique_ptr<T>>& src)
{
    std::vector<std::unique_ptr<T>> result;
    result.reserve(src.size());

    for (const auto& item : src)
        result.push_back(clone_unique(item));

    return result;
}

struct ProductionNode;

struct ASTVisitor;

struct ElementNode
{
    using Pointer = std::unique_ptr<ElementNode>;

    enum class Cardinality { ONCE, OPTIONAL, ZERO_OR_MORE, ONE_OR_MORE };

    explicit ElementNode( Cardinality card ) : card(card) {}
    virtual ~ElementNode() = default;

    virtual Pointer clone() const = 0;
    virtual void accept( ASTVisitor& visitor ) = 0;

    Cardinality card;
};

struct GroupNode : ElementNode
{
   explicit GroupNode(std::unique_ptr<ProductionNode> p, ElementNode::Cardinality card = ElementNode::Cardinality::ONCE)
        : ElementNode(card), inner(std::move(p)) { }

    Pointer clone() const override { return std::make_unique<GroupNode>( clone_unique(inner) ); }
    void accept( ASTVisitor& visitor ) override;

    std::unique_ptr<ProductionNode> inner;
};

struct SymbolNode : ElementNode
{
   explicit SymbolNode(Token token, ElementNode::Cardinality card = ElementNode::Cardinality::ONCE)
        : ElementNode(card), token(std::move(token)) { }

    Pointer clone() const override { return std::make_unique<SymbolNode>( token, card ); }
    void accept( ASTVisitor& visitor ) override;

   Token token;
};

struct PartNode
{
    using Pointer = std::unique_ptr<PartNode>;

    explicit PartNode() = default;
    virtual ~PartNode() = default;

    virtual Pointer clone() const = 0;
    virtual void accept( ASTVisitor& visitor ) = 0;
};

struct SubPartNode : PartNode
{
    explicit SubPartNode(std::vector<ElementNode::Pointer> elems)
        : element(std::move(elems)) {}

    Pointer clone() const override { return std::make_unique<SubPartNode>( clone_vector(element) ); }
    void accept( ASTVisitor& visitor ) override;

    std::vector<ElementNode::Pointer> element;
};

struct AlternatePartsNode : PartNode
{
    explicit AlternatePartsNode(std::vector<PartNode::Pointer> parts)
        : subparts(std::move(parts)) {}

    Pointer clone() const override { return std::make_unique<AlternatePartsNode>( clone_vector(subparts) ); }
    void accept( ASTVisitor& visitor ) override;

    std::vector<PartNode::Pointer> subparts;
};

struct ProductionNode
{
    explicit ProductionNode(PartNode::Pointer p) : content(std::move(p)) {}

    using Pointer = std::unique_ptr<ProductionNode>;

    Pointer clone() const { return std::make_unique<ProductionNode>( clone_unique(content) ); }
    void accept( ASTVisitor& visitor );

    PartNode::Pointer content;
};

struct RuleNode
{
    RuleNode(std::string n, ProductionNode::Pointer p)
        : name(std::move(n)), production(std::move(p)) {}

    using Pointer = std::unique_ptr<RuleNode>;

    RuleNode clone() const { return RuleNode( name, clone_unique(production) ); }
    void accept( ASTVisitor& visitor ) const;

    std::string name;
    ProductionNode::Pointer production;
};

struct SyntaxTree
{
    SyntaxTree() = default;

    SyntaxTree(const SyntaxTree& other) : rules( clone_vector(other.rules) ) {}
    SyntaxTree& operator=( const SyntaxTree& other )
    {
        if( this != &other )
            rules = clone_vector( other.rules );
        return *this;
    }
    SyntaxTree(SyntaxTree&&) noexcept = default;
    SyntaxTree& operator=(SyntaxTree&&) noexcept = default;

    void accept( ASTVisitor& visitor ) const;

    std::vector<RuleNode> rules;
};


struct ASTVisitor
{
    virtual ~ASTVisitor() = default;

    virtual void visit( const SymbolNode& symbol ) {};
    virtual void pre_symbol( const SymbolNode& symbol ) {};
    virtual void post_symbol( const SymbolNode& symbol ) {};
    virtual void pre_group( const GroupNode& group ) {};
    virtual void post_group( const GroupNode& group ) {};
    virtual void pre_elements( const SubPartNode& subpart ) {};
    virtual void post_elements( const SubPartNode& subpart ) {};
    virtual void pre_alternates( const AlternatePartsNode& alternates ) {};
    virtual void post_alternates( const AlternatePartsNode& alternates ) {};
    virtual void pre_production( const ProductionNode& production ) {};
    virtual void post_production( const ProductionNode& production ) {};
    virtual void pre_rules( const RuleNode& rule ) {};
    virtual void post_rules( const RuleNode& rule ) {};
    virtual void pre_syntax( const SyntaxTree& grammar ) {};
    virtual void post_syntax( const SyntaxTree& grammar ) {};
};

