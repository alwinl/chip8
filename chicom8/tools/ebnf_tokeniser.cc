/*
 * ebnf_tokeniser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_tokeniser.h"

#include <regex>
#include <fstream>

std::ostream& operator<<( std::ostream& os, const Token::Type& type )
{
    switch( type ) {
    case Token::Type::COMMENT: os << "COMMENT"; break;
    case Token::Type::WHITESPACE: os << "WHITESPACE"; break;
    case Token::Type::NONTERMINAL: os << "NONTERMINAL"; break;
    case Token::Type::COLON_EQ: os << "COLON_EQ"; break;
    case Token::Type::BRACKET: os << "BRACKET"; break;
    case Token::Type::MODIFIER: os << "MODIFIER"; break;
    case Token::Type::PIPE: os << "PIPE"; break;
    case Token::Type::REGEX: os << "REGEX"; break;
    case Token::Type::END_OF_PRODUCTION: os << "END_OF_PRODUCTION"; break;
    case Token::Type::STRING_LITERAL: os << "STRING_LITERAL"; break;
    case Token::Type::END_OF_INPUT: os << "END_OF_INPUT"; break;
    case Token::Type::INVALID: os << "INVALID"; break;
    }
    return os;
}

std::ostream& operator<<( std::ostream& os, const Token& token )
{
    os << token.type << "(" << token.lexeme << ")";
    return os;
}

std::ostream& operator<<( std::ostream& os, const Tokens& tokens )
{
    bool first = true;
    for( const auto& token : tokens ) {
        if( first )
            first = false;
        else
            os << ", ";

        os << token;
    }

    return os;
}



struct TokenMatcher
{
    std::regex pattern;
    Token::Type type;
    bool skip_token;
};

std::vector<TokenMatcher> match_set =
{
    TokenMatcher{ std::regex(R"(^//[^\n]*)"), Token::Type::COMMENT, false},
    TokenMatcher{ std::regex(R"(^/\*([^*]|\*[^/])*\*/)"), Token::Type::COMMENT, false},
    TokenMatcher{ std::regex(R"(^\s+)"), Token::Type::WHITESPACE, true },
    TokenMatcher{ std::regex(R"(^<[^>]*>)"), Token::Type::NONTERMINAL, false },
    TokenMatcher{ std::regex(R"(^::=)"), Token::Type::COLON_EQ, false },
    TokenMatcher{ std::regex(R"(^[()])"), Token::Type::BRACKET, false },
    TokenMatcher{ std::regex(R"(^[?+*])"), Token::Type::MODIFIER, false },
    TokenMatcher{ std::regex(R"(^\|)"), Token::Type::PIPE, false },
    TokenMatcher{ std::regex(R"(^/[^/]*/)"), Token::Type::REGEX, false },
    TokenMatcher{ std::regex(R"(^;)"), Token::Type::END_OF_PRODUCTION, false },
    TokenMatcher{ std::regex(R"(^"[^"]*")"), Token::Type::STRING_LITERAL, false },
};

Tokeniser::Tokeniser( std::filesystem::path file_path )
{
    std::ifstream src(file_path);
    std::ostringstream dst;

    dst << src.rdbuf();

    source = dst.str();
}

Token Tokeniser::next_token()
{
    if( cursor >= source.length() )
        return Token{ Token::Type::END_OF_INPUT, "" };

    auto begin = source.cbegin() + cursor;
    auto end = source.cend();
    std::smatch match;

    for( const auto& match_entry : match_set ) {

        if( std::regex_search(begin, end, match, match_entry.pattern) && match.position() == 0 ) {

            std::string lexeme = match[0];

            cursor += lexeme.length();

            if( match_entry.skip_token )
                return next_token();

            return Token{ match_entry.type, lexeme };
        }
    }

    std::string lexeme( 1, *begin );

    cursor += lexeme.length();

    return Token{ Token::Type::INVALID, lexeme };
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

