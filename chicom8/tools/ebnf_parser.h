/*
 * ebnf_parser.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_tokeniser.h"


class Parser
{
public:
    Parser( const Tokens& tokens ) : tokens(tokens ), cursor( tokens.begin() ) {}
    Parser( const std::string& source ) : tokens( Tokeniser( source ).tokenise_all() ), cursor( tokens.begin() )  {}
    Parser( std::filesystem::path file_path ) : tokens( Tokeniser( file_path ).tokenise_all() ), cursor( tokens.begin() ) {}

    Rule next_rule();
    Grammar parse_all();

private:
    const Tokens tokens;
    Tokens::const_iterator cursor;

    void forward_cursor();
    const Token& peek() { return (cursor != tokens.end()) ? *cursor : tokens.back(); }
    bool match(Token::Type type, std::string lexeme = "" );
    Token consume(Token::Type type, const std::string& message);
    Token consume(Token::Type type, const std::string& lexeme, const std::string& message);

    Production::Pointer  parse_production();
    Part::Pointer        parse_part();
    Element::Pointer     parse_element();
    Element::Cardinality parse_cardinal();
};
