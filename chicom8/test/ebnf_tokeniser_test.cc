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


TEST(EBNFTokeniserUnitTest, RecognisesNonterminal)
{
    Tokeniser lexer(std::string( "<expr>") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::NONTERMINAL, "expr", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesColonEq)
{
    Tokeniser lexer(std::string( "::=") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::COLON_EQ, "::=", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesOpenBracket)
{
    Tokeniser lexer(std::string( "(") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::OPENBRACKET, "(", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesCloseBracket)
{
    Tokeniser lexer(std::string( ")") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::CLOSEBRACKET, ")", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesPipe)
{
    Tokeniser lexer(std::string( "|") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::PIPE, "|", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesEndOfProduction)
{
    Tokeniser lexer(std::string( ";") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::END_OF_PRODUCTION, ";", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesTokenProduction)
{
    Tokeniser lexer(std::string( "KEYWORD(\"if\")") );
    EXPECT_EQ( lexer.next_token(), Token( Token::Type::TOKEN_PRODUCTION, "KEYWORD(\"if\")", 1, 1));
}

TEST(EBNFTokeniserUnitTest, RecognisesModifiers)
{
    EXPECT_EQ( Tokeniser(std::string( "*") ).next_token(), Token( Token::Type::MODIFIER, "*", 1, 1));
    EXPECT_EQ( Tokeniser(std::string( "?") ).next_token(), Token( Token::Type::MODIFIER, "?", 1, 1));
    EXPECT_EQ( Tokeniser(std::string( "+") ).next_token(), Token( Token::Type::MODIFIER, "+", 1, 1));
}


TEST(EBNFTokeniserIntegrationTest, TokenisesSimpleRule)
{
    std::string src = "<expr> ::= <term> | <factor> ;";
    Tokeniser tokenizer(src);

    Tokens tokens = tokenizer.tokenise_all();

    Tokens expected = {
        { Token::Type::NONTERMINAL, "expr", 1, 1 },
        { Token::Type::COLON_EQ, "::=", 1, 8 },
        { Token::Type::NONTERMINAL, "term", 1, 12 },
        { Token::Type::PIPE, "|", 1, 19 },
        { Token::Type::NONTERMINAL, "factor", 1, 21 },
        { Token::Type::END_OF_PRODUCTION, ";", 1, 30 },
        { Token::Type::END_OF_INPUT, "", 1, 31 }
    };

    EXPECT_EQ(tokens, expected);
}

TEST(EBNFTokeniserIntegrationTest, TokenisesChiComGrammar)
{
    std::filesystem::path file_name( "../../../chicom8/src/chicom8.bnf");
    Tokeniser tokenizer(file_name);

    try {
        Tokens tokens = tokenizer.tokenise_all();
    }
    catch(...) {
        FAIL() << "Did not expect to throw an exception tokenising 'chicom8.bnf'";
    }
}
