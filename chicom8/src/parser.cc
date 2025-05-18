/*
 * parser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "parser.h"

Parser::Parser(Tokeniser tokeniser) : tokeniser( std::move(tokeniser) )
{
    current = tokeniser.next_token();
}

// std::unique_ptr<Stmt> Parser::parse_statement()
// {
    // if (match(Token::Type::KEYWORD)) {
    //     Token name = consume(Token::Type::Identifier, "Expected variable name.");
    //     consume(Token::Type::Assign, "Expected '=' after variable name.");
    //     auto value = parse_expression();
    //     consume(Token::Type::Semicolon, "Expected ';' after expression.");
    //     return std::make_unique<LetStmt>(name.lexeme, std::move(value));
    // }

//     throw std::runtime_error("Expected statement.");
// }

// std::unique_ptr<Expr> Parser::parse_expression()
// {
    // if (current.type == Token::Type::NUMBER) {

    //     int value = std::stoi(current.lexeme);
    //     current = tokeniser.next_token();
    //     return std::make_unique<NumberLiteral>(value);

    // } else if (current.type == Token::Type::IDENTIFIER) {

    //     std::string name = current.lexeme;
    //     current = tokeniser.next_token();
    //     return std::make_unique<Variable>(name);
        
    // }

//     throw std::runtime_error("Expected expression.");
// }

void Parser::advance()
{
    current = tokeniser.next_token();
}

bool Parser::match(Token::Type type)
{
    if( current.type == type ) {

        current = tokeniser.next_token();
        return true;
    }

    return false;
}

Token Parser::consume(Token::Type type, const std::string& message)
{
    if( current.type == type ) {

        Token token = current;
        current = tokeniser.next_token();
        return token;
    }

    throw std::runtime_error("Parse error: " + message);
}