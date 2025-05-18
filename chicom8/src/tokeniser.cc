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
#include <regex>
#include <vector>
#include <unordered_map>

struct TokenMatcher
{
    std::string pattern;
    TokenType type;
    bool skip_token;
};


std::unordered_map<TokenType, std::string> type_strings =
{
    { TokenType::KEYWORD, "KEYWORD" },
    { TokenType::IDENTIFIER, "IDENTIFIER" },
    { TokenType::NUMBER, "NUMBER" },
    { TokenType::OPERATOR, "OPERATOR" },
    { TokenType::PUNCTUATION, "PUNCTUATION" },
    { TokenType::STRING_LITERAL, "STRING_LITERAL" },
    { TokenType::COMMENT, "COMMENT" },
    { TokenType::WHITESPACE, "WHITESPACE" },
    { TokenType::END_OF_INPUT, "END_OF_INPUT" },
    { TokenType::INVALID, "INVALID" },
};

std::vector<TokenMatcher> match_set =
{
    TokenMatcher{ R"(^//[^\n]*)", TokenType::COMMENT, false},
    TokenMatcher{ R"(^/\*([^*]|\*[^/])*\*/)", TokenType::COMMENT, false},
    TokenMatcher{ R"(^\s+)", TokenType::WHITESPACE, true },
    TokenMatcher{ R"(^==|!=|<=|>=|&&|\|\||\+=|\-=|\*=|/=|\+\+|\-\-)", TokenType::OPERATOR, false },
    TokenMatcher{ R"(^[+\-*\/=<>!&|])", TokenType::OPERATOR, false },
    TokenMatcher{ R"(^let|var)", TokenType::KEYWORD, false},
    TokenMatcher{ R"(^[a-zA-Z_][a-zA-Z0-9_]*)", TokenType::IDENTIFIER, false },
    TokenMatcher{ R"(^"([^*]|\*[^/])*")", TokenType::STRING_LITERAL, false},
    TokenMatcher{ R"(^[.,;()\[\]{}])", TokenType::PUNCTUATION, false },
    TokenMatcher{ R"(^\d+)", TokenType::NUMBER, false},
};

std::ostream& operator<<(std::ostream& os, TokenType type)
{
    os << type_strings[type];
    return os;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "Token(" << token.type << ", " << token.lexeme << ", " << token.line << ", " << token.column << ")";
    return os;
}


Token Tokeniser::next_token()
{
    if( cursor >= source.length() )
        return Token { TokenType::END_OF_INPUT, "", line, column };

    auto begin = source.cbegin() + cursor;
    auto end = source.cend();
    std::smatch match;

    for (const auto& match_entry : match_set) {

        std::regex token_regex(match_entry.pattern);

        if (std::regex_search(begin, end, match, token_regex) && match.position() == 0) {

            std::string lexeme = match[0];
            auto tok_line = line;
            auto tok_column = column;

            update_position_tracking( lexeme );

            if( match_entry.skip_token )
                return next_token();

            return Token{ match_entry.type, lexeme, tok_line, tok_column };
        }
    }

    std::string lexeme( 1, *begin );
    auto tok_line = line;
    auto tok_column = column;

    update_position_tracking( lexeme );

    return Token{ Token::Type::INVALID, lexeme, tok_line, tok_column };
}

Tokens Tokeniser::tokenise_all()
{
    Tokens tokens;
    Token token{ Token::Type::INVALID, "" };

    while( token.type != Token::Type::END_OF_INPUT ) {

        token = next_token();
        tokens.push_back( token );
    }

    return tokens;
}

void Tokeniser::update_position_tracking( std::string lexeme )
{
    for( char c : lexeme ) {
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }

    cursor += lexeme.length();
}
