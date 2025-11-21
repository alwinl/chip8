/*
 * ebnf_parser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <stdexcept>

#include "ebnf_parser.h"

void Parser::forward_cursor()
{
    ++cursor;

    while( peek().type == Token::Type::COMMENT )
        ++cursor; // consume the comment
}

bool Parser::match( Token::Type type, std::string lexeme )
{
    if( peek().type != type )
        return false;

    if( !lexeme.empty() && peek().lexeme != lexeme )
        return false;

    return true;
}

Token Parser::consume( Token::Type type, const std::string &lexeme, const std::string &message )
{
    if( match( type, lexeme ) ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error(
        "FileName:" + std::to_string(peek().line) + ":" + std::to_string(peek().column)
        + ": Parse error: " + message + " (expected '" + lexeme + "', got '" + peek().lexeme + "')" );
}

SyntaxTree Parser::parse_all()
{
    SyntaxTree grammar;

    while( match( Token::Type::COMMENT ) )
        ++cursor;

    while( ! match( Token::Type::END_OF_INPUT ) )
        grammar.rules.push_back( next_rule() );

    return grammar;
}

RuleNode Parser::next_rule()
{
    Token name = consume(Token::Type::NONTERMINAL, "", "not a valid rule name" );
    consume( Token::Type::COLON_EQ, "::=", "missing operator");

    std::unique_ptr<ProductionNode> production = parse_production();

    consume( Token::Type::END_OF_PRODUCTION, ";", "missing operator");

    return RuleNode { name.lexeme, std::move( production) };
}

std::unique_ptr<ProductionNode> Parser::parse_production()
{
    PartNode::Pointer part = parse_part();

    if( ! match(Token::Type::PIPE) )
        return std::make_unique<ProductionNode>( std::move(part) );

    std::vector<PartNode::Pointer> subparts;

    subparts.push_back( std::move(part) );

    while( match(Token::Type::PIPE) ) {
        consume( Token::Type::PIPE, "|", "missing symbol" );
        subparts.push_back( parse_part() );
    }

    PartNode::Pointer content = std::make_unique<AlternatePartsNode>( std::move(subparts) );

    return std::make_unique<ProductionNode>( std::move(content) );
}

PartNode::Pointer Parser::parse_part()
{
    std::vector<ElementNode::Pointer> element_list;

    element_list.push_back( std::move( parse_element() ) );

    // a list of elements finishes with a pipe, end of production, modifier or end of input
    while( ! match(Token::Type::PIPE) && ! match(Token::Type::END_OF_PRODUCTION) && !match(Token::Type::CLOSEBRACKET) && !match(Token::Type::END_OF_INPUT) )
        element_list.push_back( std::move( parse_element() ) );

    return std::make_unique<SubPartNode>( std::move( element_list ) );
}


ElementNode::Pointer Parser::parse_element()
{
    ElementNode::Pointer element;

    if( match( Token::Type::OPENBRACKET ) ) {
        consume( Token::Type::OPENBRACKET, "(", "missing symbol" );
        element = std::move( std::make_unique<GroupNode>( parse_production(), ElementNode::Cardinality::ONCE ) );
        consume( Token::Type::CLOSEBRACKET, ")", "missing symbol" );
    } else if( match( Token::Type::NONTERMINAL ) ) {
        Token tok = consume( Token::Type::NONTERMINAL, "", "missing identifier" );
        element = std::move( std::make_unique<SymbolNode>( tok, ElementNode::Cardinality::ONCE ) );
    } else if( match( Token::Type::TERMINAL ) ) {
        Token tok = consume( Token::Type::TERMINAL, "", "missing terminal" );
        element = std::move( std::make_unique<SymbolNode>( tok, ElementNode::Cardinality::ONCE ) );
    } else {
        // need to pick up if the previous line had a missing semi-colon
        throw std::runtime_error("Unknown token: " + peek().lexeme);
    }

    element->card = parse_cardinal();

    return element;
}

ElementNode::Cardinality Parser::parse_cardinal()
{
    if( !match( Token::Type::MODIFIER ) )
        return ElementNode::Cardinality::ONCE;

    Token mod = consume( Token::Type::MODIFIER, "", "expected modifier" );
    switch( mod.lexeme[0] ) {
    case '*': return ElementNode::Cardinality::ZERO_OR_MORE;
    case '+': return ElementNode::Cardinality::ONE_OR_MORE;
    case '?': return ElementNode::Cardinality::OPTIONAL;
    }

    throw std::runtime_error("Unknown modifier: " + mod.lexeme);
}

