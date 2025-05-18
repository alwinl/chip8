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

#include "ebnf_parser.h"

#include <stdexcept>

std::ostream& operator<<( std::ostream& os, const Symbol& symbol )
{
    if( symbol.symbol_group.empty() ) {
        const Token& token = symbol.token;
        os << token;
    } else {
        os << "\(" << symbol.symbol_group << ")";
        if( symbol.optional )
            os << "?";
        if( symbol.repeated )
            os << "*";
    }

    return os;
}

std::ostream& operator<<( std::ostream& os, const Production& production )
{
    // os << "[";

    bool first = true;
    for( const auto& symbol : production ) {
        if( first )
            first = false;
        else
            os << ", ";

        os << symbol;
    }

    // os << "]";

    return os;
}

std::ostream& operator<<( std::ostream& os, const Rule& rule )
{
    os << "\t{\n";

    os << "\t\t\"" << rule.name << "\",\n";
    os << "\t\t[\n";

    bool first = true;

    for( const auto& production : rule.productions ) {
        if( first )
            first = false;
        else
            os << ",\n";

        os << "\t\t\t" << production;
    }

    os << "\n\t\t]\n";

    os << "\t},\n";

    return os;
}

std::ostream& operator<<( std::ostream& os, const Rules& rules )
{
    os << "[\n";

    for( const auto& rule : rules )
        os << rule;

    os << "]\n";

    return os;
}



void Parser::forward_cursor()
{
    ++cursor;

    while( peek().type == Token::Type::COMMENT )
        ++cursor; // consume the comment
}


std::string Parser::parse_rule_name()
{
    auto token = peek();

    if( token.type != Token::Type::NONTERMINAL )
        throw std::runtime_error( "Expected rule name at token: " + token.lexeme + "\n");

    std::string rule_name = token.lexeme;

    rule_name.erase(0,1);
    rule_name.pop_back();

    forward_cursor();

    return rule_name;
}

void Parser::parse_colon_eq()
{
    if( peek().type != Token::Type::COLON_EQ )
        throw std::runtime_error( "Expected '::=' after rule name\n" );

    forward_cursor();
}

Symbol Parser::apply_modifier( Symbol& symbol)
{
    auto mod = peek().lexeme;
    symbol.optional = (mod == "?");
    symbol.repeated = (mod == "*");

    forward_cursor();

    return symbol;
}

Symbols Parser::parse_production( bool in_group )
{
   Symbols symbols;

    while( true ) {
        
        Token token = peek();

        if( token.type == Token::Type::END_OF_PRODUCTION || token.type == Token::Type::END_OF_INPUT )
            break;

        if( token.type == Token::Type::PIPE )
            if( !in_group )
                break;
            else {
                forward_cursor(); // consume '|'
                continue;
            }

        if( token.type == Token::Type::BRACKET && token.lexeme == ")" )            // end of group
            break;

        if( token.type == Token::Type::BRACKET && token.lexeme == "(" ) {                   // start of group

            Symbol symbol;

            forward_cursor(); // consume '('

            symbol.symbol_group = parse_production( true ); // recursive parse

            if( peek().type != Token::Type::BRACKET || peek().lexeme != ")" )
                throw std::runtime_error("Unmatched '(' in production" );

            forward_cursor(); // consume ')'

            if( peek().type == Token::Type::MODIFIER )
                apply_modifier( symbol );

            symbols.push_back( std::move(symbol) );

        } else {                                                                        // regular token

            Symbol symbol;

            if( token.type == Token::Type::NONTERMINAL ) {
                token.lexeme.erase(0,1);
                token.lexeme.pop_back();
            }

            symbol.token = token;
            forward_cursor();

            if( peek().type == Token::Type::MODIFIER )
                apply_modifier( symbol );

            symbols.push_back( std::move(symbol) );
        }
    }

    return symbols;
}

Rules Parser::parse_rules( )
{
    Rules rules;

    cursor = tokens.begin();

    while( peek().type != Token::Type::END_OF_INPUT ) {

        std::string rule_name = parse_rule_name();

        parse_colon_eq();

        Productions productions;
        Production current_production;

        while( true ) {
            
            Token tok = peek();

            if( tok.type == Token::Type::END_OF_INPUT || tok.type == Token::Type::END_OF_PRODUCTION ) 
                break;

            if( tok.type == Token::Type::PIPE ) {
                productions.push_back( current_production );
                current_production.clear();
                forward_cursor();
                continue;
            }
            
            Production parsed_symbols = parse_production( false );
            current_production.insert( current_production.end(), parsed_symbols.begin(), parsed_symbols.end() );
        }

        if( !current_production.empty() )
            productions.push_back( current_production );

        rules.push_back( Rule {rule_name, productions} );

        forward_cursor();
    }

    return rules;
}

