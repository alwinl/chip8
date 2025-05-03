/*
 * tokeniser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "tokeniser.h"



Tokeniser::Tokeniser(const std::string& source) : source(source), current(0), line(1), column(1)
{
    // Initialize the tokeniser with the source code
}

Token Tokeniser::next_token()
{
    skip_whitespace();

    if (is_at_end())
        return make_token(TokenType::Eof);

    char c = peek();

    if (std::isdigit(c))
        return number();

    if (std::isalpha(c) || c == '_')
        return identifier();

    switch (c) {
    case '=': return make_token(TokenType::Assign);
    case '+': return make_token(TokenType::Plus);
    case '-': return make_token(TokenType::Minus);
    case '*': return make_token(TokenType::Multiply);
    case '/': return make_token(TokenType::Divide);
    case '(': return make_token(TokenType::LParen);
    case ')': return make_token(TokenType::RParen);
    case ',': return make_token(TokenType::Comma);
    case ';': return make_token(TokenType::Semicolon);
    }

    return Token{ TokenType::Invalid, source.substr(current,1), line, column };
}

char Tokeniser::advance()
{
    if( is_at_end() )
        return '\0';

    column++;
    return source[current++];
}

char Tokeniser::peek()
{
    return is_at_end() ? '\0' : source[current];
}

bool Tokeniser::is_at_end()
{
    return current >= source.length();
}

void Tokeniser::skip_whitespace()
{
    while( !is_at_end() && isspace(peek()) ) {

        if( peek() == '\n' ) {
            line++;
            column = 1;
        } else
            column++;

        advance();
    }
}

Token Tokeniser::identifier() 
{
    size_t start = current;
    int start_col = column;

    while (!is_at_end() && (std::isalnum(peek()) || peek() == '_'))
        advance();

    std::string lexeme = source.substr(start, current - start);

    TokenType type = TokenType::Identifier;

    if (lexeme == "let")
        type = TokenType::Let;

    return Token{ type, lexeme, line, start_col };
}

Token Tokeniser::number()
{
    size_t start = current;
    int start_col = column;

    while (!is_at_end() && std::isdigit(peek()))
        advance();

    std::string lexeme = source.substr(start, current - start);

    return Token{ TokenType::Number, lexeme, line, start_col };
}

Token Tokeniser::make_token(TokenType type, std::string lexeme)
{
    advance();

    return Token{ type, lexeme, line, column };
}

Token Tokeniser::error_token(const std::string& message)
{
    return Token{ TokenType::Invalid, message, line, column }; 
}
