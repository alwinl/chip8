/*
 * ebnf_token.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <unordered_map>

#include "ebnf_token.h"

std::ostream& operator<<( std::ostream& os, const Token& token )
{
    static std::unordered_map<Token::Type, std::string> type_strings =
    {
        { Token::Type::COMMENT, "COMMENT" },
        { Token::Type::WHITESPACE, "WHITESPACE" },
        { Token::Type::NONTERMINAL, "NONTERMINAL" },
        { Token::Type::COLON_EQ, "COLON_EQ" },
        { Token::Type::OPENBRACKET, "OPENBRACKET" },
        { Token::Type::CLOSEBRACKET, "CLOSEBRACKET" },
        { Token::Type::MODIFIER, "MODIFIER" },
        { Token::Type::PIPE, "PIPE" },
        { Token::Type::END_OF_PRODUCTION, "END_OF_PRODUCTION" },
        { Token::Type::TERMINAL, "TERMINAL" },
        { Token::Type::END_OF_INPUT, "END_OF_INPUT" },
        { Token::Type::INVALID, "INVALID" },
    };

    os << "{ "
            "type: " << type_strings[token.type] << ", "
            "lexeme: " << token.lexeme << ", "
            "line: " << token.line << ", "
            "column: " << token.column << "}";
    return os;
}

