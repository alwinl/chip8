/*
 * tokeniser_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <gtest/gtest.h>

TEST(TokeniserTest, ParsesVariableDeclaration)
{
    std::string source = "let x = 42;";
    Tokeniser tokeniser(source);

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Let);

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Identifier);
    EXPECT_EQ(token.lexeme, "x");

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Assign);

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Number);
    EXPECT_EQ(token.lexeme, "42");

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Semicolon);

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Eof);
}

TEST(TokeniserTest, ParsesSimpleExpression)
{
    std::string source = "x = x + 1;";
    Tokeniser tokeniser(source);

    EXPECT_EQ(tokeniser.next_token().type, TokenType::Identifier); // x
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Assign);     // =
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Identifier); // x
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Plus);       // +
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Number);     // 1
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Semicolon);  // ;
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Eof);        // EOF
}

TEST(TokeniserTest, ParsesFunctionCall)
{
    std::string source = "draw(x, y);";
    Tokeniser tokeniser(source);

    EXPECT_EQ(tokeniser.next_token().type, TokenType::Identifier); // draw
    EXPECT_EQ(tokeniser.next_token().type, TokenType::LParen);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Identifier); // x
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Comma);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Identifier); // y
    EXPECT_EQ(tokeniser.next_token().type, TokenType::RParen);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Semicolon);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::Eof);
}

TEST(TokeniserTest, DetectsInvalidToken)
{
    std::string source = "@";
    Tokeniser tokeniser(source);

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::Invalid);
    EXPECT_EQ(token.lexeme, "@");
}
