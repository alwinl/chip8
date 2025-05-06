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
    EXPECT_EQ(token.type, TokenType::KEYWORD);

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::IDENTIFIER);
    EXPECT_EQ(token.lexeme, "X");

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::OPERATOR);

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::NUMBER);
    EXPECT_EQ(token.lexeme, "42");

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::PUNCTUATION);

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::END_OF_INPUT);
}

TEST(TokeniserTest, ParsesSimpleExpression)
{
    std::string source = "x = x + 1;";
    Tokeniser tokeniser(source);

    EXPECT_EQ(tokeniser.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(tokeniser.next_token().type, TokenType::OPERATOR);     // =
    EXPECT_EQ(tokeniser.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(tokeniser.next_token().type, TokenType::OPERATOR);       // +
    EXPECT_EQ(tokeniser.next_token().type, TokenType::NUMBER);     // 1
    EXPECT_EQ(tokeniser.next_token().type, TokenType::PUNCTUATION);  // ;
    EXPECT_EQ(tokeniser.next_token().type, TokenType::END_OF_INPUT);        // EOF
}

TEST(TokeniserTest, ParsesFunctionCall)
{
    std::string source = "draw(x, y);";
    Tokeniser tokeniser(source);

    EXPECT_EQ(tokeniser.next_token().type, TokenType::IDENTIFIER); // draw
    EXPECT_EQ(tokeniser.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::IDENTIFIER); // x
    EXPECT_EQ(tokeniser.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::IDENTIFIER); // y
    EXPECT_EQ(tokeniser.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::PUNCTUATION);
    EXPECT_EQ(tokeniser.next_token().type, TokenType::END_OF_INPUT);
}

TEST(TokeniserTest, DetectsInvalidToken)
{
    std::string source = "@";
    Tokeniser tokeniser(source);

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::INVALID);
    EXPECT_EQ(token.lexeme, "@");
}

TEST(TokeniserTest, ParsesStringLiteral)
{
    std::string source = "LET x = \"hello\";";
    Tokeniser tokeniser(source);

    // Skip LET x = tokens
    for (int i = 0; i < 3; ++i) tokeniser.next_token();

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::STRING_LITERAL);
    EXPECT_EQ(token.lexeme, "hello");  // Assuming your fix strips quotes

    token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::PUNCTUATION);
    EXPECT_EQ(token.lexeme, ";");
}

TEST(TokeniserTest, HandlesUnterminatedStringLiteral)
{
    std::string source = "LET x = \"unterminated";
    Tokeniser tokeniser(source);

    // Skip LET x =
    for (int i = 0; i < 3; ++i) tokeniser.next_token();

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::INVALID);
    EXPECT_TRUE(token.lexeme.find("Unterminated") != std::string::npos);
}

TEST(TokeniserTest, HandlesInvalidCharacter)
{
    std::string source = "LET x = 42 @;";
    Tokeniser tokeniser(source);

    // Skip LET x = 42
    for (int i = 0; i < 5; ++i) tokeniser.next_token();

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::INVALID);
    EXPECT_EQ(token.lexeme, "@");
}

TEST(TokeniserTest, ParsesIdentifiersWithUnderscores)
{
    std::string source = "LET my_var = 1;";
    Tokeniser tokeniser(source);

    // Skip LET
    tokeniser.next_token();

    Token token = tokeniser.next_token();
    EXPECT_EQ(token.type, TokenType::IDENTIFIER);
    EXPECT_EQ(token.lexeme, "MY_VAR");
}

// Helper function to extract all tokens from the source
std::vector<Token> extract_tokens(const std::string& src)
{
    Tokeniser tokenizer(src);
    std::vector<Token> tokens;
    while (true) {
        Token t = tokenizer.next_token();
        if (t.type == TokenType::END_OF_INPUT) break;
        if (t.type != TokenType::INVALID) tokens.push_back(t);  // Ignore invalids for these tests
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
        {TokenType::IDENTIFIER, "A"},
        {TokenType::OPERATOR, "+="},
        {TokenType::IDENTIFIER, "B"},
        {TokenType::OPERATOR, "&&"},
        {TokenType::IDENTIFIER, "C"},
        {TokenType::OPERATOR, "=="},
        {TokenType::IDENTIFIER, "D"},
        {TokenType::PUNCTUATION, ";"}
    };

    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i].first);
        EXPECT_EQ(tokens[i].lexeme, expected[i].second);
    }
}

TEST(TokeniserTest, LineCommentsAreSkipped)
{
    auto tokens = extract_tokens("x = 1; // this is a comment\ny = 2;");
    std::vector<std::pair<TokenType, std::string>> expected = {
        {TokenType::IDENTIFIER, "X"},
        {TokenType::OPERATOR, "="},
        {TokenType::NUMBER, "1"},
        {TokenType::PUNCTUATION, ";"},
        {TokenType::COMMENT, " this is a comment"},
        {TokenType::IDENTIFIER, "Y"},
        {TokenType::OPERATOR, "="},
        {TokenType::NUMBER, "2"},
        {TokenType::PUNCTUATION, ";"}
    };

    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i].first);
        EXPECT_EQ(tokens[i].lexeme, expected[i].second);
    }
}

TEST(TokeniserTest, BlockCommentsAreSkipped) {
    auto tokens = extract_tokens("x = 1; /* multi-line \n comment */ y = 2;");
    std::vector<std::pair<TokenType, std::string>> expected = {
        {TokenType::IDENTIFIER, "X"},
        {TokenType::OPERATOR, "="},
        {TokenType::NUMBER, "1"},
        {TokenType::PUNCTUATION, ";"},
        {TokenType::COMMENT, " multi-line \n comment "},
        {TokenType::IDENTIFIER, "Y"},
        {TokenType::OPERATOR, "="},
        {TokenType::NUMBER, "2"},
        {TokenType::PUNCTUATION, ";"}
    };

    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i].first);
        EXPECT_EQ(tokens[i].lexeme, expected[i].second);
    }
}