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

#include <unordered_set>
#include <algorithm>

const std::unordered_set<std::string> keywords = {
    "VAR",
    "LET"
};

std::string punctuation = ".,;()[]{}";
std::string operators = "+-*/=<>!&|";
const std::unordered_set<std::string> multi_char_operators = {
    "==", "!=", "<=", ">=", "&&", "||", "+=", "-=", "*=", "/=", "++", "--"
};



// Always ensure that the input finishes with an empty line
Tokeniser::Tokeniser(const std::string& source) : source(source + '\n'), current(0), line(1), column(1)
{
}

Token Tokeniser::next_token()
{
    ParsingState state = ParsingState::NONE;

    while( current < source.length() ) {

        char current_char = source[current];

        if( state == ParsingState::NONE ) {

            if( process_eol( current_char ) )
                continue;

            if( process_whitespace( current_char ) )
                continue;

            if( process_alpha( current_char ) ) {
                state = ParsingState::IDENTIFIER;
                continue;
            }

            if( process_number( current_char ) ) {
                state = ParsingState::NUMBER;
                continue;
            }

            if( process_string_literal( current_char ) ) {
                state = ParsingState::STRING_LITERAL;
                continue;
            }

            if( process_operator( current_char ) ) {
                state = ParsingState::OPERATOR;
                continue;
            }

            if( process_punctuation( current_char ) )
                return punctuation_token();

            return Token{ TokenType::INVALID, std::string(1, current_char), line, column };
        }

        if( state == ParsingState::IDENTIFIER ) {

            if( collecting_identifier( current_char ) )
                continue;

            state = ParsingState::NONE;

            return identifier_token();
        }

        if( state == ParsingState::NUMBER ) {

            if( collecting_number( current_char ) )
                continue;

            state = ParsingState::NONE;

            return number_token();
        }

        if( state == ParsingState::STRING_LITERAL ) {

            if( collecting_string_literal(current_char) )
                continue;

            state = ParsingState::NONE;

            return string_literal_token();
        }

        if( state == ParsingState::OPERATOR ) {

            // there are a number of options here
            // 1 it could be a single char operator
            // 2 it could be a multi char operator
            // 3 it could the start of a line comment
            // 4 it could the start of a star comment

            if( is_line_comment( current_char) ) {
                state = ParsingState::LINE_COMMENT;
                continue;
            }

            if( is_star_comment( current_char) ) {
                state = ParsingState::STAR_COMMENT;
                continue;
            }

            if( collecting_operator(current_char) )
                continue;

            state = ParsingState::NONE;

            return operator_token();
        }

        if( state == ParsingState::STAR_COMMENT ) {

            if( collecting_star_comment( current_char ) )
                continue;

            state = ParsingState::NONE;

            return comment_token();
        }

        if( state == ParsingState::LINE_COMMENT ) {

            if( collecting_line_comment( current_char ) )
                continue;

            state = ParsingState::NONE;

            return line_comment_token();
        }

        return Token{ TokenType::INVALID, "Parser out of sequence", line, column };
    }

    if( state == ParsingState::STRING_LITERAL )
        return Token{ TokenType::INVALID, "Unterminated string literal", line, lexeme_column };

    if( state == ParsingState::STAR_COMMENT )
        return Token{ TokenType::INVALID, "Unterminated star comment", line, lexeme_column };

    return Token{ TokenType::END_OF_INPUT, "", line, column };
}

bool Tokeniser::process_eol(char current_char)
{
    if( current_char != '\n' )
        return false;

    line++;
    column = 1;
    ++current;
    return true;
}

bool Tokeniser::process_whitespace(char current_char)
{
    if( !isspace(current_char) )
        return false;

    ++current;
    ++column;
    return true;
}

bool Tokeniser::process_alpha( char current_char )
{
    if( !isalpha(current_char) )
        return false;

    lexeme_start = current;
    lexeme_column = column;
    ++current;
    ++column;
    return true;
}

bool Tokeniser::collecting_identifier( char current_char )
{
    if( !isalnum(current_char) && current_char != '_' )
        return false;

    ++current;
    ++column;

    return true;
}

Token Tokeniser::identifier_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start);

    for (auto & c: lexeme)
        c = toupper(c);

    if( std::find( keywords.begin(), keywords.end(), lexeme) != keywords.end() )
        return Token{TokenType::KEYWORD, lexeme, line, lexeme_column};

    return Token{TokenType::IDENTIFIER, lexeme, line, lexeme_column};
}

bool Tokeniser::process_number( char current_char )
{
    if( !isdigit(current_char) )
        return false;

    lexeme_start = current;
    lexeme_column = column;
    ++current;
    ++column;
    return true;
}

bool Tokeniser::collecting_number( char current_char )
{
    if( !isdigit(current_char) )
        return false;

    ++current;
    ++column;
    return true;
}

Token Tokeniser::number_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start);

    return Token{TokenType::NUMBER, lexeme, line, lexeme_column};
}

bool Tokeniser::process_string_literal( char current_char )
{
    if( current_char != '\"' )
        return false;

    lexeme_start = current+1;
    lexeme_column = column;
    ++current;
    ++column;
    return true;
}

bool Tokeniser::collecting_string_literal( char current_char )
{
    if( current_char == '\"' )
        return false;

    ++current;
    ++column;
    return true;
}

Token Tokeniser::string_literal_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start);

    ++current;
    ++column;

    return Token{TokenType::STRING_LITERAL, lexeme, line, lexeme_column};
}

bool Tokeniser::process_operator( char current_char )
{
    if( std::find(operators.begin(), operators.end(), current_char) == operators.end() )
        return false;

    lexeme_start = current;
    lexeme_column = column;
    ++current;
    ++column;

    return true;
}

bool Tokeniser::collecting_operator( char current_char )
{
    std::string two_chars = source.substr(lexeme_start, 2);
    if( ! multi_char_operators.count(two_chars) )       // we have a single char operator
        return false;

    ++current;
    ++column;
    return false;                                       // we have a multi byte operator
}

Token Tokeniser::operator_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start);

    return Token{TokenType::OPERATOR, lexeme, line, lexeme_column};
}

bool Tokeniser::is_line_comment( char current_char )
{
    std::string lexeme = source.substr(lexeme_start, 2);
    if( lexeme != "//" )
        return false;

    ++current;
    ++column;
    lexeme_start = current;

    return true;
}

bool Tokeniser::is_star_comment( char current_char )
{
    std::string lexeme = source.substr(lexeme_start, 2);
    if( lexeme != "/*" )
        return false;

    ++current;
    ++column;
    lexeme_start = current;

    return true;
}

bool Tokeniser::collecting_line_comment( char current_char )
{
    if( current_char == '\n' )
        return false;

    ++current;
    ++column;
    return true;
}

Token Tokeniser::line_comment_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start);

    return Token{TokenType::COMMENT, lexeme, line, lexeme_column};
}

bool Tokeniser::collecting_star_comment( char current_char )
{
    std::string lexeme = source.substr(current - 1, 2);
    if( lexeme == "*/") {
        ++current;
        ++column;
        return false;
    }

    ++current;
    ++column;
    return true;
}

Token Tokeniser::comment_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start - 2);   // we need to discard the '*/'

    return Token{TokenType::COMMENT, lexeme, line, lexeme_column};
}

bool Tokeniser::process_punctuation( char current_char )
{
    if( std::find(punctuation.begin(), punctuation.end(), current_char) == punctuation.end() )
        return false;

    lexeme_start = current;
    lexeme_column = column;
    ++current;
    ++column;

    return true;
}

Token Tokeniser::punctuation_token()
{
    std::string lexeme = source.substr(lexeme_start, current - lexeme_start);

    return Token{TokenType::PUNCTUATION, lexeme, line, lexeme_column};
}



std::ostream& operator<<(std::ostream& os, TokenType type)
{
    switch (type) {
    case TokenType::KEYWORD: os << "KEYWORD"; break;
    case TokenType::IDENTIFIER: os << "IDENTIFIER"; break;
    case TokenType::NUMBER: os << "NUMBER"; break;
    case TokenType::OPERATOR: os << "OPERATOR"; break;
    case TokenType::PUNCTUATION: os << "PUNCTUATION"; break;
    case TokenType::STRING_LITERAL: os << "STRING_LITERAL"; break;
    case TokenType::END_OF_INPUT: os << "END_OF_INPUT"; break;
    case TokenType::INVALID: os << "INVALID"; break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "Token(" << token.type << ", " << token.lexeme << ", " << token.line << ", " << token.column << ")";
    return os;
}