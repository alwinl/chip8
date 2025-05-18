/*
 * ebnf_parser_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

#include "ebnf_tokeniser.h"
#include "ebnf_parser.h"
#include "ebnf_generator.h"


// Helper to tokenize a string (not from file)
std::vector<Token> tokenize_string( const char * src )
{
    return Tokeniser( std::string(src) ).tokenise_all();
}

TEST(EbnfCompilerTest, ParsesSingleRule)
{
    auto tokens = tokenize_string("<expr> ::= \"42\" ;");
    auto rules = Parser(tokens).parse_rules();

    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].name, "expr");

    ASSERT_EQ(rules[0].productions.size(), 1);
    ASSERT_EQ(rules[0].productions[0].size(), 1);
    EXPECT_EQ(rules[0].productions[0][0].token.lexeme, "\"42\"");
}

TEST(EbnfCompilerTest, ParsesAlternatives)
{
    auto tokens = tokenize_string("<expr> ::= \"42\" | <ident> ;");
    auto rules = Parser(tokens).parse_rules();

    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].name, "expr");

    ASSERT_EQ(rules[0].productions.size(), 2);
    ASSERT_EQ(rules[0].productions[0].size(), 1);
    EXPECT_EQ(rules[0].productions[0][0].token.lexeme, "\"42\"");

    ASSERT_EQ(rules[0].productions[1].size(), 1);
    EXPECT_EQ(rules[0].productions[1][0].token.lexeme, "ident");
}

TEST(EbnfCompilerTest, ParsesMultipleRules)
{
    auto tokens = tokenize_string("<expr> ::= \"42\" ; <ident> ::= \"x\" ;");
    auto rules = Parser(tokens).parse_rules();

    ASSERT_EQ(rules.size(), 2);
    EXPECT_EQ(rules[0].name, "expr");
    EXPECT_EQ(rules[1].name, "ident");
}

TEST(EbnfCompilerTest, HandlesStringLiteralsAndNonterminals)
{
    auto tokens = tokenize_string("<rule> ::= \"foo\" <bar> ;");
    auto rules = Parser(tokens).parse_rules();

    ASSERT_EQ(rules.size(), 1);
    ASSERT_EQ(rules[0].productions.size(), 1);
    EXPECT_EQ(rules[0].productions[0][0].token.lexeme, "\"foo\"");
    EXPECT_EQ(rules[0].productions[0][1].token.lexeme, "bar");
}

TEST(EbnfCompilerTest, HandlesInvalidInputGracefully)
{
    auto tokens = tokenize_string("<expr> \"42\" ;");    // Missing ::= 
    
    // Should throw exception
    try {
        auto rules = Parser(tokens).parse_rules();
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}


