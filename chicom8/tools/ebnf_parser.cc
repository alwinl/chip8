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

Token Parser::consume( Token::Type type, const std::string& message )
{
    if( match( type ) ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error("Parse error: " + message);
}

Token Parser::consume( Token::Type type, const std::string &lexeme, const std::string &message )
{
    if( match( type, lexeme ) ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error(
        "Parse error: " + message +
        "(expected '" + lexeme + "', got '" + peek().lexeme + "')" +
        " at line " + std::to_string(peek().line) );
}

Grammar Parser::parse_all()
{
    Grammar grammar;

    while( match( Token::Type::COMMENT ) )
        ++cursor;

    while( ! match( Token::Type::END_OF_INPUT ) )
        grammar.rules.push_back( next_rule() );

    return grammar;
}

Rule Parser::next_rule()
{
    Token name = consume(Token::Type::NONTERMINAL, "Expected rule name" );
    consume( Token::Type::COLON_EQ, "Expected '::='");

    std::unique_ptr<Production> production = parse_production();

    consume( Token::Type::END_OF_PRODUCTION, "Expected ';");

    return Rule { name.lexeme, std::move( production) };
}

std::unique_ptr<Production> Parser::parse_production()
{
    Part::Pointer part = parse_part();

    if( ! match(Token::Type::PIPE) )
        return std::make_unique<Production>( std::move(part) );

    std::vector<Part::Pointer> subparts;

    subparts.push_back( std::move(part) );

    while( match(Token::Type::PIPE) ) {
        consume( Token::Type::PIPE, "" );
        subparts.push_back( parse_part() );
    }

    Part::Pointer content = std::make_unique<AlternateParts>( std::move(subparts) );

    return std::make_unique<Production>( std::move(content) );
}

Part::Pointer Parser::parse_part()
{
    std::vector<Element::Pointer> element_list;

    element_list.push_back( std::move( parse_element() ) );

    // a list of elements finishes with a pipe, end of production, modifier or end of input
    while( ! match(Token::Type::PIPE) && ! match(Token::Type::END_OF_PRODUCTION) && !match(Token::Type::CLOSEBRACKET) && !match(Token::Type::END_OF_INPUT) )
        element_list.push_back( std::move( parse_element() ) );

    return std::make_unique<SubPart>( std::move( element_list ) );
}


Element::Pointer Parser::parse_element()
{
    Element::Pointer element;

    if( match( Token::Type::OPENBRACKET ) ) {
        consume( Token::Type::OPENBRACKET, "(" );
        element = std::move( std::make_unique<Group>( parse_production(), Cardinality::ONCE ) );
        consume( Token::Type::CLOSEBRACKET, ")", "Expected closing brace" );
    } else if( match( Token::Type::NONTERMINAL ) ) {
        Token tok = consume( Token::Type::NONTERMINAL, "" );
        element = std::move( std::make_unique<Symbol>( tok, Cardinality::ONCE ) );
    } else if( match( Token::Type::TOKEN_PRODUCTION ) ) {
        Token tok = consume( Token::Type::TOKEN_PRODUCTION, "" );
        element = std::move( std::make_unique<Symbol>( tok, Cardinality::ONCE ) );
    } else {
        throw std::runtime_error("Unknown Token: " + peek().lexeme);
    }

    element->card = parse_cardinal();

    return element;
}

Cardinality Parser::parse_cardinal()
{
    if( !match( Token::Type::MODIFIER ) )
        return Cardinality::ONCE;

    Token mod = consume( Token::Type::MODIFIER, "Expected modifier" );
    switch( mod.lexeme[0] ) {
    case '*': return Cardinality::ZERO_OR_MORE;
    case '+': return Cardinality::ONE_OR_MORE;
    case '?': return Cardinality::OPTIONAL;
    }

    throw std::runtime_error("Unknown cardinality modifier: " + mod.lexeme);
}

void Group::accept( ASTVisitor& visitor ) { visitor.visit( *this); };
void Symbol::accept( ASTVisitor& visitor ) { visitor.visit( *this); };
void SubPart::accept( ASTVisitor& visitor ) { visitor.visit( *this); };
void AlternateParts::accept( ASTVisitor& visitor ) { visitor.visit( *this); };
void Production::accept( ASTVisitor& visitor ) { visitor.visit( *this); };
void Rule::accept( ASTVisitor& visitor ) { visitor.visit( *this); };

