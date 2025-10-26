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
#include <fstream>

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
    TokenMatcher{ std::regex(R"(^==|!=|<=|>=|&&|\|\||\+=|\-=|\*=|/=|\+\+|\-\-)"), Token::Type::OPERATOR, false },
    TokenMatcher{ std::regex(R"(^[+\-*\/=<>!&|])"), Token::Type::OPERATOR, false },
    TokenMatcher{ std::regex(R"(^nibble|byte|snack|number|bool|key|sprite)"), Token::Type::TYPE_KEYWORD, false},
    TokenMatcher{ std::regex(R"(^let|var|func)"), Token::Type::KEYWORD, false},
    TokenMatcher{ std::regex(R"(^[a-zA-Z_][a-zA-Z0-9_]*)"), Token::Type::IDENTIFIER, false },
    TokenMatcher{ std::regex(R"(^"([^*]|\*[^/])*")"), Token::Type::STRING_LITERAL, false},
    TokenMatcher{ std::regex(R"(^[.,:;()\[\]{}])"), Token::Type::PUNCTUATION, false },
    TokenMatcher{ std::regex(R"(^0x[0-9A-Fa-f]+)"), Token::Type::NUMBER, false },
    TokenMatcher{ std::regex(R"(^\d+)"), Token::Type::NUMBER, false},
};

std::string escape_string( const std::string& input)
{
    std::string escaped = "\"";
    for( char c : input ) {
        switch (c) {
        case '"': escaped += "\\\""; break;
        case '\\': escaped += "\\\\"; break;
        case '\n': escaped += "\\n"; break;
        case '\r': escaped += "\\r"; break;
        case '\t': escaped += "\\t"; break;
        default: escaped += c; break;
        }
    }
    escaped += "\"";
    return escaped;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    static std::unordered_map<Token::Type, std::string> type_strings =
    {
        { Token::Type::KEYWORD, "KEYWORD" },
        { Token::Type::TYPE_KEYWORD, "TYPE_KEYWORD" },
        { Token::Type::IDENTIFIER, "IDENTIFIER" },
        { Token::Type::NUMBER, "NUMBER" },
        { Token::Type::OPERATOR, "OPERATOR" },
        { Token::Type::PUNCTUATION, "PUNCTUATION" },
        { Token::Type::STRING_LITERAL, "STRING_LITERAL" },
        { Token::Type::COMMENT, "COMMENT" },
        { Token::Type::WHITESPACE, "WHITESPACE" },
        { Token::Type::END_OF_INPUT, "END_OF_INPUT" },
        { Token::Type::INVALID, "INVALID" },
    };

    os << "{ "
            "\"type\": \"" << type_strings[token.type] << "\", "
            "\"lexeme\": " << escape_string(token.lexeme) << ", "
            "\"line\": " << token.line << ", "
            "\"column\": " << token.column << "}";
    return os;
}

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
        return Token { Token::Type::END_OF_INPUT, "", line, column };

    auto begin = source.cbegin() + cursor;
    auto end = source.cend();
    std::smatch match;

    for (const auto& match_entry : match_set) {

        if (std::regex_search(begin, end, match, match_entry.pattern) && match.position() == 0) {

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
