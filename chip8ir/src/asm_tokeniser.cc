 /*
  * asm_tokeniser.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "asm_tokeniser.h"

#include <regex>
#include <unordered_map>
#include <assert.h>

struct TokenMatcher
{
    std::regex pattern;
    Token::Type type;
    bool skip_token;
};

std::vector<TokenMatcher> match_set =
{
    TokenMatcher{ std::regex(R"(^;[^\n]*)"), Token::Type::COMMENT, false},
    TokenMatcher{ std::regex(R"(^\s+)"), Token::Type::WHITESPACE, true },
	TokenMatcher{ std::regex(R"(^(=|EQU)\b)"), Token::Type::ASSIGNMENT, false },
    TokenMatcher{ std::regex(R"(^[A-Za-z_][A-Za-z_0-9]*:)"), Token::Type::LABEL, false },
	TokenMatcher{ std::regex(R"(^\.[A-Za-z_][A-Za-z_0-9]*)"), Token::Type::DIRECTIVE, false },
	TokenMatcher{ std::regex(R"(^[A-Za-z_][A-Za-z_0-9]*)"), Token::Type::IDENTIFIER, false },
    TokenMatcher{ std::regex(R"(^(0x[0-9A-Fa-f]+|\d+))"), Token::Type::NUMBER, false },
    TokenMatcher{ std::regex(R"(^,)"), Token::Type::COMMA, true }
};

Tokens ASMTokeniser::tokenise( ASMSource source )
{
    Tokens tokens;

	for( const auto& line : source )
        tokenise_line( line.text, line.line_no, tokens );

	tokens.push_back({ Token::Type::END_OF_INPUT, "", 0, 0 });

    return tokens;
}

void ASMTokeniser::tokenise_line( const std::string& input, int line_no, Tokens & tokens )
{
	int cursor = 0;
	auto end = input.cend();

	while( cursor < input.size() ) {

		auto begin = input.cbegin() + cursor;
		std::smatch match;
		bool matched = false;

		for( const auto& match_entry : match_set ) {

			if( ! std::regex_search( begin, end, match, match_entry.pattern) )
				continue;

			assert( match.position() == 0 );
			assert( match.length() > 0 );

			std::string lexeme = match[0];

			if( ! match_entry.skip_token )
				tokens.push_back( { match_entry.type, post_process( match_entry.type, lexeme ), line_no, cursor + 1 } );

			cursor += lexeme.size();
			matched = true;
			break;
		}

		if( !matched ) {
            tokens.push_back( { Token::Type::INVALID, std::string(1, input[cursor]), line_no, cursor + 1 } );
            cursor++;
        }
	}
}

std::string ASMTokeniser::post_process( Token::Type type, std::string lexeme )
{
	auto make_upper = [](unsigned char c){ return std::toupper(c); };

	if( type == Token::Type::LABEL && lexeme.back() == ':' )
        lexeme.pop_back();

	if( (type == Token::Type::IDENTIFIER) || (type == Token::Type::DIRECTIVE) || (type == Token::Type::ASSIGNMENT) )
		std::transform( lexeme.begin(), lexeme.end(), lexeme.begin(), make_upper );

    return lexeme;
}
