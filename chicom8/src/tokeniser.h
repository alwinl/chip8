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

enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    PUNCTUATION,
    STRING_LITERAL,
    COMMENT,
    WHITESPACE,
    END_OF_INPUT,
    INVALID
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;
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

    enum class ParsingState {
        NONE,
        IDENTIFIER,
        NUMBER,
        STRING_LITERAL,
        OPERATOR,
        LINE_COMMENT,
        STAR_COMMENT
    };

    int lexeme_start;
    int lexeme_end;
    int lexeme_column;

    bool process_eol(char current_char);
    bool process_whitespace(char current_char);

    bool process_alpha(char current_char);
    bool collecting_identifier( char current_char );
    Token identifier_token();

    bool process_number( char current_char );
    bool collecting_number( char current_char );
    Token number_token();

    bool process_string_literal( char current_char );
    bool collecting_string_literal( char current_char );
    Token string_literal_token();

    bool process_operator( char current_char );
    bool collecting_operator( char current_char );
    Token operator_token();

    bool is_line_comment( char current_char );
    bool collecting_line_comment( char current_char );
    Token line_comment_token();

    bool is_star_comment( char current_char );
    bool collecting_star_comment( char current_char );
    Token comment_token();

    bool process_punctuation( char current_char );
    Token punctuation_token();
};

class Tokeniser2
{
public:
    Tokeniser2(const std::string& source) : source(source) {};
	Token next_token();

private:
    std::string source;
    int cursor = 0;
    int line = 1;
    int column = 1;

    void update_position_tracking( std::string lexeme );
};

std::ostream& operator<<(std::ostream& os, TokenType type);
std::ostream& operator<<(std::ostream& os, const Token& token);
