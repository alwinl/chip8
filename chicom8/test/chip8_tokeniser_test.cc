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
    Tokeniser lexer( std::string("// this is a comment\n42"));

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COMMENT, "// this is a comment", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NUMBER, "42", 2, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 2, 3 ) );
}

TEST(TokeniserTest, ParsesMultiLineComment)
{
    Tokeniser lexer( std::string("/* comment \n over two lines */\nhello"));

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COMMENT, "/* comment \n over two lines */", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "hello", 3, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 3, 6 ) );
}

TEST(TokeniserTest, SkipsWhitespace)
{
    Tokeniser lexer( std::string("  \t\n  123"));

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NUMBER, "123", 2, 3 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 2, 6 ) );
}

TEST(TokeniserTest, ParsesNumber )
{
    Tokeniser lexer(  std::string("42") );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NUMBER, "42", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 3 ) );
}

TEST(TokeniserTest, ParsesIdentifier )
{
    Tokeniser lexer(  std::string("hello") );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "hello", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 6 ) );
}

TEST(TokeniserTest, ParsesStringLiteral)
{
    Tokeniser lexer( std::string("\"hello\""));

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::STRING_LITERAL, R"("hello")", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 8 ) );
}

// Helper function to extract all tokens from the source
std::vector<Token> extract_tokens(const std::string& src)
{
    Tokeniser lexer(src);
    std::vector<Token> tokens;

    while( true ) {
        Token t = lexer.next_token();

        if (t.type == Token::Type::END_OF_INPUT)
            break;

        if (t.type != Token::Type::INVALID)
            tokens.push_back(t);  // Ignore invalids so all can be checked
    }
    return tokens;
}

TEST(TokeniserTest, SingleCharacterOperators)
{
    auto tokens = extract_tokens("= < > + - * /");
    std::vector<std::string> expected = {"=", "<", ">", "+", "-", "*", "/"};

    ASSERT_EQ(tokens.size(), expected.size());

    for( size_t i = 0; i < expected.size(); ++i )
        EXPECT_EQ( tokens[i], Token( Token::Type::OPERATOR, expected[i], 1, 1 + 2*i ) );
}

TEST(TokeniserTest, MultiCharacterOperators)
{
    auto tokens = extract_tokens("== != <= >= && || ++ --");
    std::vector<std::string> expected = {"==", "!=", "<=", ">=", "&&", "||", "++", "--"};

    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ( tokens[i], Token( Token::Type::OPERATOR, expected[i], 1, 1 + 3*i ) );
}

TEST(TokeniserTest, MixedOperatorsWithIdentifiers)
{
    Tokeniser lexer(  std::string("a+=b && c==d;") );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "a", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::OPERATOR, "+=", 1, 2 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "b", 1, 4 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::OPERATOR, "&&", 1, 6 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "c", 1, 9 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::OPERATOR, "==", 1, 10 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "d", 1, 12 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::PUNCTUATION, ";", 1, 13 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 14 ) );
}

TEST(TokeniserTest, InvalidToken)
{
    Tokeniser lexer( std::string("@"));

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::INVALID, "@", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 2 ) );
}

TEST(TokeniserTest, HandlesInvalidCharacter)
{
    Tokeniser lexer( std::string("var x = 42 @;"));

    // Skip let x = 42
    for (int i = 0; i < 4; ++i)
        lexer.next_token();

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::INVALID, "@", 1, 12 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::PUNCTUATION, ";", 1, 13 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 14 ) );
}

TEST(TokeniserTest, GetTwoTokens )
{
    Tokeniser lexer(  std::string("hello 42") );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "hello", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NUMBER, "42", 1, 7 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 9 ) );
}

TEST(TokeniserTest, ParsesSimpleExpression)
{
    Tokeniser lexer(  std::string("x = x + 1;") );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "x", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::OPERATOR, "=", 1, 3 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::IDENTIFIER, "x", 1, 5 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::OPERATOR, "+", 1, 7 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NUMBER, "1", 1, 9 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::PUNCTUATION, ";", 1, 10 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 11 ) );
}

TEST(TokeniserTest, ParsesFunctionCall)
{
    Tokeniser lexer( std::string("do_draw(x, y);") );

    EXPECT_EQ(lexer.next_token(), Token( Token::Type::IDENTIFIER, "do_draw", 1, 1 ) ); // draw
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::PUNCTUATION, "(", 1, 8 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::IDENTIFIER, "x", 1, 9 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::PUNCTUATION, ",", 1, 10 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::IDENTIFIER, "y", 1, 12 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::PUNCTUATION, ")", 1, 13 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::PUNCTUATION, ";", 1, 14 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 15 ) );
}

TEST(TokeniserTest, ParsesVariableDeclaration)
{
    Tokeniser lexer( std::string("var x = 42;") );

    EXPECT_EQ(lexer.next_token(), Token( Token::Type::KEYWORD, "var", 1, 1 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::IDENTIFIER, "x", 1, 5 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::OPERATOR, "=", 1, 7 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::NUMBER, "42", 1, 9 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::PUNCTUATION, ";", 1, 11 ) );
    EXPECT_EQ(lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 12 ) );

}

TEST(TokeniserTest, HandlesUnterminatedStringLiteral)
{
    Tokeniser lexer( std::string("var x = \"unterminated") );

    // Skip var x =
    for (int i = 0; i < 3; ++i)
        lexer.next_token();

    EXPECT_EQ(lexer.next_token(), Token( Token::Type::INVALID, "\"", 1, 9 ) );
}

TEST(TokeniserTest, ParsesIdentifiersWithUnderscores)
{
    Tokeniser lexer( std::string("var my_var = 1;") );

    // Skip var
    lexer.next_token();

    EXPECT_EQ(lexer.next_token(), Token( Token::Type::IDENTIFIER, "my_var", 1, 5 ) );
}

TEST(TokeniserTest, KEY_1_is_identifier)
{
    Tokeniser lexer( std::string("KEY_1") );

    EXPECT_EQ(lexer.next_token(), Token( Token::Type::IDENTIFIER, "KEY_1", 1, 1 ) );
}

TEST(TokeniserTest, TestSnippet)
{
    ::testing::internal::CaptureStdout();

    Tokeniser lexer( std::string(
R"(func foo() {
    x = keydown KEY_1;
})") );

    for (Token tok = lexer.next_token();
        tok.type != Token::Type::END_OF_INPUT;
        tok = lexer.next_token())
    {
        std::cout << tok << "\n";
    }

    std::string output = ::testing::internal::GetCapturedStdout();
    std::cerr << output;
}
