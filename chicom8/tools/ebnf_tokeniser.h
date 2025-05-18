/*
 * ebnf_tokeniser.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <filesystem>

struct Token
{
    enum class Type { COMMENT, WHITESPACE, NONTERMINAL, COLON_EQ, BRACKET, MODIFIER, PIPE, END_OF_PRODUCTION, STRING_LITERAL,  END_OF_INPUT, INVALID };

    Type type;
    std::string lexeme;
};

using Tokens = std::vector<Token>;

std::ostream& operator<<( std::ostream& os, const Token& token );
std::ostream& operator<<( std::ostream& os, const Tokens& tokens );

class Tokeniser
{
public:
    Tokeniser( std::string source ) : source(source) {};
    Tokeniser( std::filesystem::path file_path );

    Token next_token();
    Tokens tokenise_all();

private:
    std::string source;
    int cursor = 0;
};
