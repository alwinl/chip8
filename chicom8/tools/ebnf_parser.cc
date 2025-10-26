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
    if( symbol.symbol_group.empty() )
        os << "        Symbol: " << symbol.token;
    else
        os << "        \(" << symbol.symbol_group << ")";

    if( symbol.optional )
        os << "?";

    if( symbol.repeated )
        os << "*";

    return os;
}

std::ostream& operator<<( std::ostream& os, const Part& part )
{
    os << "      Part : [\n";

    bool first = true;
    for( const auto& symbol : part ) {
        if( first )
            first = false;
        else
            os << ",\n";

        os << symbol;
    }

    os << "\n      ]";

    return os;
}

std::ostream& operator<<( std::ostream& os, const Production& production )
{
    os << "    production : [\n";

    bool first = true;
    for( const auto& part : production ) {
        if( first )
            first = false;
        else
            os << ",\n";

        os << part;
    }

    os << "\n    ]\n";

    return os;
}

std::ostream& operator<<( std::ostream& os, const Rule& rule )
{
    os << "  Rule : {\n";
    os << "    name : " << rule.name << ",\n" ;
    os << rule.production;
    os << "  }";

    return os;
}

std::ostream& operator<<( std::ostream& os, const Grammar& grammar )
{
    os << "Grammar: [\n";

    bool first = true;
    for( const auto& rule : grammar ) {
        if( first )
            first = false;
        else
            os << ",\n";

        os << rule;
    }

    os << "\n]";

    return os;
}



std::string sanitise_non_terminal( const Token& token )
{
    if( token.type != Token::Type::NONTERMINAL )
        return token.lexeme;

    std::string lexeme = token.lexeme;

    // Remove angle brackets
    lexeme.erase(0,1);
    lexeme.pop_back();

    return lexeme;
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

    std::string rule_name = sanitise_non_terminal( token );

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
    if( peek().type != Token::Type::MODIFIER )
        return symbol;

    auto mod = peek().lexeme;
    symbol.optional = (mod == "?");
    symbol.repeated = (mod == "*");

    forward_cursor();

    return symbol;
}

Symbol Parser::parse_regular_token( const Token& token )
{
    Symbol symbol;

    symbol.token = token;

    symbol.token.lexeme = sanitise_non_terminal( token );

    forward_cursor();

    apply_modifier( symbol );

    return symbol;
}

Symbol Parser::parse_group_token( )
{
    Symbol symbol;

    forward_cursor(); // consume '('

    symbol.symbol_group = parse_part( true ); // recursive parse

    if( peek().type != Token::Type::BRACKET || peek().lexeme != ")" )
        throw std::runtime_error("Unmatched '(' in production" );

    forward_cursor(); // consume ')'

    apply_modifier( symbol );

    return symbol;
}

Part Parser::parse_part( bool in_group )
{
    Part part;

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

            Symbol symbol = parse_group_token( );

            part.push_back( std::move(symbol) );

        } else {                                                                        // regular token

            Symbol symbol = parse_regular_token( token );

            part.push_back( std::move(symbol) );
        }
    }

    return part;
}

Production Parser::parse_production()
{
    Production production;
    Part part;

    while( true ) {

        Token tok = peek();

        if( tok.type == Token::Type::END_OF_INPUT || tok.type == Token::Type::END_OF_PRODUCTION )
            break;

        if( tok.type == Token::Type::PIPE ) {
            production.push_back( part );
            part.clear();
            forward_cursor();
            continue;
        }

        Part parsed_symbols = parse_part( false );
        part.insert( part.end(), parsed_symbols.begin(), parsed_symbols.end() );
    }

    if( !part.empty() )
        production.push_back( part );

    forward_cursor();

    return production;
}

Rule Parser::parse_rule()
{
    Rule rule;

    rule.name = parse_rule_name();
    parse_colon_eq();
    rule.production = parse_production();

    return rule;
}

Grammar Parser::syntax_tree( )
{
    Grammar grammar;

    cursor = tokens.begin();

    while( peek().type != Token::Type::END_OF_INPUT )
        grammar.push_back( parse_rule() );

    return grammar;
}

