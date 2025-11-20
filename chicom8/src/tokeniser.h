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
#include <ostream>
#include <filesystem>
#include <vector>

struct Token
{
    enum class Type {
        KEYWORD, TYPE_KEYWORD, IDENTIFIER, NUMBER, OPERATOR, PUNCTUATION, STRING_LITERAL,
        COMMENT, WHITESPACE, END_OF_INPUT, INVALID
    };

    Type type;
    std::string lexeme;
    int line;
    int column;

    auto operator<=>(const Token&) const = default;
};

using Tokens = std::vector<Token>;

std::ostream& operator<<( std::ostream& os, const Token& token );

class Tokeniser
{
public:
    Tokeniser(const std::string& source) : source(source) {};
    Tokeniser( std::filesystem::path file_path );

    Token next_token();
    Tokens tokenise_all();

private:
    std::string source;
    int cursor = 0;
    int line = 1;
    int column = 1;

    void update_position_tracking( std::string lexeme );
};
