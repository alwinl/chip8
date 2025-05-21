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

TEST(EBNFTokenTest, TokenStreamOutputSingleToken)
{
    Token token{Token::Type::NONTERMINAL, "Expr"};
    std::stringstream ss;
    ss << token;
    
    EXPECT_EQ(ss.str(), "NONTERMINAL(Expr)");
}

TEST(EBNFTokenTest, TokenStreamOutputMultipleTokens)
{
    Tokens tokens = {
        {Token::Type::NONTERMINAL, "Expr"},
        {Token::Type::COLON_EQ, "::="},
        {Token::Type::STRING_LITERAL, "\"a\""},
    };

    std::stringstream ss;
    ss << tokens;

    EXPECT_EQ(ss.str(), "NONTERMINAL(Expr), COLON_EQ(::=), STRING_LITERAL(\"a\")");
}

TEST(EBNFTokenTest, TokeniseAllTokenTypes)
{
    std::string input = "<expr> ::= ( <something>? | <another>+ )* \"astring\" /[a-f]/ ; /*comment*/";
    Tokens tokens = Tokeniser(input).tokenise_all();

    std::string expected = "NONTERMINAL(<expr>), COLON_EQ(::=), BRACKET((), NONTERMINAL(<something>), "
                         "MODIFIER(?), PIPE(|), NONTERMINAL(<another>), MODIFIER(+), BRACKET()), "
                         "MODIFIER(*), STRING_LITERAL(\"astring\"), REGEX(/[a-f]/), END_OF_PRODUCTION(;), "
                         "COMMENT(/*comment*/), END_OF_INPUT()";

    std::stringstream ss;
    ss << tokens;

    EXPECT_EQ( ss.str(), expected );

}

TEST(EBNFTokeniserTest, TokeniseSimpleProduction)
{
    std::string input = "<Expr> ::= \"a\"";
    Tokeniser tokenizer(input);

    Tokens tokens = tokenizer.tokenise_all();

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, Token::Type::NONTERMINAL);
    EXPECT_EQ(tokens[0].lexeme, "<Expr>");

    EXPECT_EQ(tokens[1].type, Token::Type::COLON_EQ);
    EXPECT_EQ(tokens[1].lexeme, "::=");

    EXPECT_EQ(tokens[2].type, Token::Type::STRING_LITERAL);
    EXPECT_EQ(tokens[2].lexeme, "\"a\"");

    EXPECT_EQ(tokens[3].type, Token::Type::END_OF_INPUT);
}

TEST(EBNFTokeniserTest, TokeniseHandlesWhitespaceAndComments)
{
    std::string input = "Expr ::= \"a\" // comment";
    Tokeniser tokenizer(input);

    Tokens tokens = tokenizer.tokenise_all();

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
    std::string input = "<hex>     ::= /0x[0-9a-fA-F]+/";

    Tokens tokens = Tokeniser(input).tokenise_all();

    std::stringstream ss;
    ss << tokens;

    EXPECT_EQ(ss.str(), "NONTERMINAL(<hex>), COLON_EQ(::=), REGEX(/0x[0-9a-fA-F]+/), END_OF_INPUT()");
}