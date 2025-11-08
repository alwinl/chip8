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

struct Production;

struct ASTVisitor;

enum class Cardinality { ONCE, OPTIONAL, ZERO_OR_MORE, ONE_OR_MORE };

struct Element
{
    using Pointer = std::unique_ptr<Element>;

    explicit Element( Cardinality card ) : card(card) {}
    virtual ~Element() = default;

    virtual Pointer clone() const = 0;
    virtual void accept( ASTVisitor& visitor ) = 0;

    Cardinality card;
};

struct Group : Element
{
   explicit Group(std::unique_ptr<Production> p, Cardinality card = Cardinality::ONCE)
        : Element(card), inner(std::move(p)) { }

    Pointer clone() const override { return std::make_unique<Group>( clone_unique(inner) ); }
    void accept( ASTVisitor& visitor ) override;

    std::unique_ptr<Production> inner;
};

struct Symbol : Element
{
   explicit Symbol(Token token, Cardinality card = Cardinality::ONCE)
        : Element(card), token(std::move(token)) { }

    Pointer clone() const override { return std::make_unique<Symbol>( token, card ); }
    void accept( ASTVisitor& visitor ) override;

   Token token;
};

struct Part
{
    using Pointer = std::unique_ptr<Part>;

    explicit Part() = default;
    virtual ~Part() = default;

    virtual Pointer clone() const = 0;
    virtual void accept( ASTVisitor& visitor ) = 0;
};

struct SubPart : Part
{
    explicit SubPart(std::vector<Element::Pointer> elems)
        : element(std::move(elems)) {}

    Pointer clone() const override { return std::make_unique<SubPart>( clone_vector(element) ); }
    void accept( ASTVisitor& visitor ) override;

    std::vector<Element::Pointer> element;
};

struct AlternateParts : Part
{
    explicit AlternateParts(std::vector<Part::Pointer> parts)
        : subparts(std::move(parts)) {}

    Pointer clone() const override { return std::make_unique<AlternateParts>( clone_vector(subparts) ); }
    void accept( ASTVisitor& visitor ) override;

    std::vector<Part::Pointer> subparts;
};

struct Production
{
    explicit Production(Part::Pointer p) : content(std::move(p)) {}

    using Pointer = std::unique_ptr<Production>;

    Pointer clone() const { return std::make_unique<Production>( clone_unique(content) ); }
    void accept( ASTVisitor& visitor );

    Part::Pointer content;
};

struct Rule
{
    Rule(std::string n, Production::Pointer p)
        : name(std::move(n)), production(std::move(p)) {}

    using Pointer = std::unique_ptr<Rule>;

    Rule clone() const { return Rule( name, clone_unique(production) ); }
    void accept( ASTVisitor& visitor );

    std::string name;
    Production::Pointer production;
};

using Rules = std::vector<Rule>;

struct Grammar
{
    Grammar() = default;

    Grammar(const Grammar& other) : rules( clone_vector(other.rules) ) {}
    Grammar& operator=( const Grammar& other )
    {
        if( this != &other )
            rules = clone_vector( other.rules );
        return *this;
    }
    Grammar(Grammar&&) noexcept = default;
    Grammar& operator=(Grammar&&) noexcept = default;

    std::vector<Rule> rules;
};


struct ASTVisitor
{
    virtual ~ASTVisitor() = default;

    virtual void visit( const Group& group ) = 0;
    virtual void visit( const Symbol& symbol ) = 0;
    virtual void visit( const SubPart& subpart ) = 0;
    virtual void visit( const AlternateParts& alternates ) = 0;
    virtual void visit( const Production& production ) = 0;
    virtual void visit( const Rule& rule ) = 0;
    virtual void visit( const Grammar& grammar ) = 0;
};

