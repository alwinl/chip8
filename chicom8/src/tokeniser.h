/*
 * tokeniser.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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


enum class TokenType
{
    Let,
    Identifier,
    Number,
    Assign,
    Plus,
    Minus,
    Multiply,
    Divide,
    LParen,
    Comma,
    RParen,
    Semicolon,
    Eof,
    Invalid
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
};

class Tokeniser
{
public:
    Tokeniser(const std::string& source);
    Token next_token();

private:
    std::string source;
    size_t current;
    int line;
    int column;

    char advance();
    char peek();
    bool is_at_end();

    void skip_whitespace();
    Token identifier();
    Token number();
    Token make_token(TokenType type, std::string lexeme = "");
    Token error_token(const std::string& message);
};