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

TEST(Tokeniser2Test, ParsesSingleLineComment)
{
    Tokeniser2 lexer("// this is a comment\n42");

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

TEST(Tokeniser2Test, ParsesMultiLineComment)
{
    Tokeniser2 lexer("/* comment \n over two lines */\nhello");

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

TEST(Tokeniser2Test, ParsesWhitespace)
{
    Tokeniser2 lexer("  \t\n  123");
    Token tok = lexer.next_token();

    EXPECT_EQ(tok.type, TokenType::NUMBER);
    EXPECT_EQ(tok.lexeme, "123");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 2);  // If newline is correctly tracked
}

TEST(Tokeniser2Test, ParsesNumber )
{
    Tokeniser2 lexer( "42" );

    Token tok = lexer.next_token();

    EXPECT_EQ( tok.type, TokenType::NUMBER );
    EXPECT_EQ( tok.lexeme, "42" );

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
}

TEST(Tokeniser2Test, ParsesIdentifier )
{
    Tokeniser2 lexer( "hello" );

    Token tok = lexer.next_token();
    EXPECT_EQ( tok.type, TokenType::IDENTIFIER );
    EXPECT_EQ( tok.lexeme, "hello" );

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
}

TEST(Tokeniser2Test, ParsesStringLiteral)
{
    Tokeniser2 lexer("\"hello\"");

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tok.lexeme, R"("hello")");

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 8);
}

TEST(Tokeniser2Test, InvalidToken)
{
    Tokeniser2 lexer("@");

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::INVALID);

    tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::END_OF_INPUT);
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 2);
}

TEST(Tokeniser2Test, GetTwoTokens )
{
    Tokeniser2 lexer( "hello 42" );

    Token token = lexer.next_token();

    EXPECT_EQ( token.type, TokenType::IDENTIFIER );
    EXPECT_EQ( token.lexeme, "hello" );

    token = lexer.next_token();

    EXPECT_EQ( token.type, TokenType::NUMBER );
    EXPECT_EQ( token.lexeme, "42" );
}

TEST(Tokeniser2Test, ParsesSimpleExpression)
{
    Tokeniser2 lexer( "x = x + 1;" );

    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(lexer.next_token().type, TokenType::OPERATOR);     // =
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(lexer.next_token().type, TokenType::OPERATOR);       // +
    EXPECT_EQ(lexer.next_token().type, TokenType::NUMBER);     // 1
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);  // ;
    EXPECT_EQ(lexer.next_token().type, TokenType::END_OF_INPUT);        // EOF
}

TEST(Tokeniser2Test, ParsesVariableDeclaration)
{
    Tokeniser2 lexer( "let x = 42;" );

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

TEST(Tokeniser2Test, ParsesFunctionCall)
{
    Tokeniser2 lexer("draw(x, y);");

    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // draw
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::IDENTIFIER); // y
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(lexer.next_token().type, TokenType::END_OF_INPUT);
}

TEST(Tokeniser2Test, HandlesUnterminatedStringLiteral)
{
    Tokeniser lexer("LET x = \"unterminated");

    // Skip LET x =
    for (int i = 0; i < 3; ++i)
        lexer.next_token();

    Token tok = lexer.next_token();
    EXPECT_EQ(tok.type, TokenType::INVALID);
    EXPECT_TRUE(tok.lexeme.find("Unterminated") != std::string::npos);
}


