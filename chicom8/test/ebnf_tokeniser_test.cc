/*
 * ebnf_tokeniser_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <string>
#include <vector>
#include <filesystem>

#include "ebnf_tokeniser.h"

TEST(EBNFTokenTest, TokeniseAllTokenTypes)
{
    Tokeniser lexer( std::string( "<expr> ::= ( <something>? | <another>+ )* \"astring\" /[a-f]/ ; /*comment*/") );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "<expr>", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COLON_EQ, "::=", 1, 8 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::BRACKET, "(", 1, 12 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "<something>", 1, 14 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::MODIFIER, "?", 1, 25 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::PIPE, "|", 1, 27 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "<another>", 1, 29 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::MODIFIER, "+", 1, 38 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::BRACKET, ")", 1, 40 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::MODIFIER, "*", 1, 41 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::STRING_LITERAL, "\"astring\"", 1, 43 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::REGEX, "/[a-f]/", 1, 53 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_PRODUCTION, ";", 1, 61 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COMMENT, "/*comment*/", 1, 63 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 74 ) );
}

TEST(EBNFTokeniserTest, TokeniseSimpleProduction)
{
    Tokeniser lexer( std::string("<Expr> ::= \"a\"") );

    Tokens tokens = lexer.tokenise_all();

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ( tokens[0], Token( Token::Type::NONTERMINAL, "<Expr>", 1, 1 ) );
    EXPECT_EQ( tokens[1], Token( Token::Type::COLON_EQ, "::=", 1, 8 ) );
    EXPECT_EQ( tokens[2], Token( Token::Type::STRING_LITERAL, "\"a\"", 1, 12) );
    EXPECT_EQ( tokens[3], Token( Token::Type::END_OF_INPUT, "", 1, 15 ) );
}

TEST(EBNFTokeniserTest, TokeniseHandlesWhitespaceAndComments)
{
    Tokeniser lexer( std::string("Expr ::= \"a\" // comment"));

    Tokens tokens = lexer.tokenise_all();

    bool foundComment = false;
    for (const auto& token : tokens) {
        if (token.type == Token::Type::COMMENT) {
            foundComment = true;
            EXPECT_EQ(token.lexeme, "// comment");
        }
    }

    EXPECT_TRUE(foundComment);
}

TEST(EBNFTokeniserTest, TokeniseRegex)
{
    Tokeniser lexer( std::string( "<hex>     ::= /0x[0-9a-fA-F]+/" ) );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "<hex>", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COLON_EQ, "::=", 1, 11 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::REGEX, "/0x[0-9a-fA-F]+/", 1, 15 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 31 ) );
}

TEST(EBNFTokeniserTest, TokeniseTokenProduction)
{
    Tokeniser lexer( std::string( "TOKEN_PRODUCT" ) );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::TOKEN_PRODUCTION, "TOKEN_PRODUCT", 1, 1 ) );
}

TEST(EBNFTokeniserTest, TokeniseTokenProductionWithParam)
{
    Tokeniser lexer( std::string( "TOKEN_PRODUCT(\"-\")" ) );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::TOKEN_PRODUCTION, "TOKEN_PRODUCT(\"-\")", 1, 1 ) );
}

TEST(EBNFTokeniserTest, TokeniseTokenProductionWithClosingBracketParam)
{
    Tokeniser lexer( std::string( "TOKEN_PRODUCT(\")\")" ) );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::TOKEN_PRODUCTION, "TOKEN_PRODUCT(\")\")", 1, 1 ) );
}

TEST(EBNFTokeniserTest, TokeniseTokenProductionIntegration)
{
    Tokeniser lexer( std::string( "<typed_identifier> ::= IDENTIFIER PUNCTUATION(\":\") <type> ;" ) );

    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "<typed_identifier>", 1, 1 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COLON_EQ, "::=", 1, 20 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::TOKEN_PRODUCTION, "IDENTIFIER", 1, 24 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::TOKEN_PRODUCTION, "PUNCTUATION(\":\")", 1, 35 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "<type>", 1, 52 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_PRODUCTION, ";", 1, 59 ) );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_INPUT, "", 1, 60 ) );
}
