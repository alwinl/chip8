/*
 * tokeniser2_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <gtest/gtest.h>

#include "tokeniser.h"

TEST(TokeniserTest, ParsesSingleLineComment)
{
    Tokeniser lexer("// this is a comment\n42");

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::COMMENT);
    EXPECT_EQ(tok.lexeme, "// this is a comment");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, "42");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 2);
}

TEST(TokeniserTest, ParsesMultiLineComment)
{
    Tokeniser lexer("/* comment \n over two lines */\nhello");

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::COMMENT);
    EXPECT_EQ(tok.lexeme, "/* comment \n over two lines */");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::IDENTIFIER);
    EXPECT_EQ(tok.lexeme, "hello");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 3);
}

TEST(TokeniserTest, ParsesWhitespace)
{
    Tokeniser lexer("  \t\n  123");
    Token tok = lexer.next_token();

    EXPECT_EQ(tok.type, TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, "123");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 2);  // If newline is correctly tracked
}

TEST(TokeniserTest, ParsesNumber )
{
    Tokeniser lexer( "42" );

    Token tok = lexer.next_token();

    EXPECT_EQ( tok.type, TokenType::NUMBER );
    EXPECT_EQ( tok.lexeme, "42" );

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
}

TEST(TokeniserTest, ParsesIdentifier )
{
    Tokeniser lexer( "hello" );

    Token tok = lexer.next_token();
    EXPECT_EQ( tok.type, TokenType::IDENTIFIER );
    EXPECT_EQ( tok.lexeme, "hello" );

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
}

TEST(TokeniserTest, ParsesStringLiteral)
{
    Tokeniser lexer("\"hello\"");

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tok.lexeme, R"("hello")");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 8);
}

// Helper function to extract all tokens from the source
std::vector<Token> extract_tokens(const std::string& src)
{
    Tokeniser lexer(src);
    std::vector<Token> tokens;

    while( true ) {
        Token t = lexer.next_token();

        if (t.type == TokenType::END_OF_INPUT)
            break;

        if (t.type != TokenType::INVALID)
            tokens.push_back(t);  // Ignore invalids so all can be checked
    }
    return tokens;
}

TEST(TokeniserTest, SingleCharacterOperators)
{
    auto tokens = extract_tokens("= < > + - * /");
    std::vector<std::string> expected = {"=", "<", ">", "+", "-", "*", "/"};

    ASSERT_EQ(tokens.size(), expected.size());

    for( size_t i = 0; i < expected.size(); ++i ) {
        EXPECT_EQ(tokens[i].type, TokenType::OPERATOR);
        EXPECT_EQ(tokens[i].lexeme, expected[i]);
    }
}

TEST(TokeniserTest, MultiCharacterOperators)
{
    auto tokens = extract_tokens("== != <= >= && || ++ --");
    std::vector<std::string> expected = {"==", "!=", "<=", ">=", "&&", "||", "++", "--"};

    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, TokenType::OPERATOR);
        EXPECT_EQ(tokens[i].lexeme, expected[i]);
    }
}

TEST(TokeniserTest, MixedOperatorsWithIdentifiers)
{
    auto tokens = extract_tokens("a+=b && c==d;");
    std::vector<std::pair<TokenType, std::string>> expected = {
        {TokenType::IDENTIFIER, "a"},
        {TokenType::OPERATOR, "+="},
        {TokenType::IDENTIFIER, "b"},
        {TokenType::OPERATOR, "&&"},
        {TokenType::IDENTIFIER, "c"},
        {TokenType::OPERATOR, "=="},
        {TokenType::IDENTIFIER, "d"},
        {TokenType::PUNCTUATION, ";"}
    };

    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i].first);
        EXPECT_EQ(tokens[i].lexeme, expected[i].second);
    }
}

TEST(TokeniserTest, InvalidToken)
{
    Tokeniser lexer("@");

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::INVALID);

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 2);
}

TEST(TokeniserTest, HandlesInvalidCharacter)
{
    Tokeniser lexer("let x = 42 @;");

    // Skip let x = 42
    for (int i = 0; i < 4; ++i) lexer.next_token();

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::INVALID);
    EXPECT_EQ(tok.lexeme, "@");
}

TEST(TokeniserTest, GetTwoTokens )
{
    Tokeniser lexer( "hello 42" );

    Token tok = lexer.next_token();

    EXPECT_EQ( tok.type, TokenType::IDENTIFIER );
    EXPECT_EQ( tok.lexeme, "hello" );

    tok = lexer.next_token();

    EXPECT_EQ( tok.type, TokenType::NUMBER );
    EXPECT_EQ( tok.lexeme, "42" );
}

TEST(TokeniserTest, ParsesSimpleExpression)
{
    Tokeniser lexer( "x = x + 1;" );

    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(lexer.next_token().type, TokenType::OPERATOR);     // =
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(lexer.next_token().type, TokenType::OPERATOR);       // +
    EXPECT_EQ(lexer.next_token().type, TokenType::NUMBER);     // 1
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);  // ;
    EXPECT_EQ(lexer.next_token().type, TokenType::END_OF_INPUT);        // EOF
}

TEST(TokeniserTest, ParsesFunctionCall)
{
    Tokeniser lexer( "draw(x, y);" );

    // EXPECT_EQ(lexer.next_token(), Token{TokenType::IDENTIFIER, "draw", 1, 1 } ); // draw
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // draw
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // y
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::END_OF_INPUT);
}

TEST(TokeniserTest, ParsesVariableDeclaration)
{
    Tokeniser lexer( "let x = 42;" );

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::KEYWORD);
    EXPECT_EQ(tok.lexeme, "let");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::IDENTIFIER);
    EXPECT_EQ(tok.lexeme, "x");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::OPERATOR);
    EXPECT_EQ(tok.lexeme, "=");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, "42");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::PUNCTUATION);
    EXPECT_EQ(tok.lexeme, ";");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
}

TEST(TokeniserTest, HandlesUnterminatedStringLiteral)
{
    Tokeniser lexer("LET x = \"unterminated");

    // Skip LET x =
    for (int i = 0; i < 3; ++i)
        lexer.next_token();

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::INVALID);
    EXPECT_EQ(tok.lexeme, "\"" );
}

TEST(TokeniserTest, ParsesIdentifiersWithUnderscores)
{
    Tokeniser lexer("let my_var = 1;");

    // Skip let
    lexer.next_token();

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::IDENTIFIER);
    EXPECT_EQ(tok.lexeme, "my_var");
}
