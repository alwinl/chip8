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
// #include "ebnf_generator.h"

TEST(EBNFParserUnit, ParsesSimpleRule)
{
    std::string source = "<expr> ::= <term> ;";
    Parser parser(source);
    SyntaxTree grammar = parser.parse_all();

    ASSERT_EQ(grammar.rules.size(), 1);
    EXPECT_EQ(grammar.rules[0].name, "expr");

    // ProductionNode → SubPartNode → SymbolNode
    auto* prod = dynamic_cast<SubPartNode*>(grammar.rules[0].production->content.get());
    ASSERT_NE(prod, nullptr);
    ASSERT_EQ(prod->element.size(), 1);

    auto* sym = dynamic_cast<SymbolNode*>(prod->element[0].get());
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->token.lexeme, "term");
}

TEST(EBNFParserUnit, ParsesAlternation)
{
    std::string source = "<expr> ::= <term> | <factor> ;";
    Parser parser(source);
    SyntaxTree grammar = parser.parse_all();

    ASSERT_EQ(grammar.rules.size(), 1);

    auto* alt = dynamic_cast<AlternatePartsNode*>(grammar.rules[0].production->content.get());
    ASSERT_NE(alt, nullptr);
    ASSERT_EQ(alt->subparts.size(), 2);

    auto* left = dynamic_cast<SubPartNode*>(alt->subparts[0].get());
    auto* right = dynamic_cast<SubPartNode*>(alt->subparts[1].get());
    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);

    auto* left_symbol = dynamic_cast<SymbolNode*>(left->element[0].get());
    auto* right_symbol = dynamic_cast<SymbolNode*>(right->element[0].get());
    ASSERT_NE(left_symbol, nullptr);
    ASSERT_NE(right_symbol, nullptr);

    EXPECT_EQ(left_symbol->token.lexeme, "term");
    EXPECT_EQ(right_symbol->token.lexeme, "factor");
}

TEST(EBNFParserUnit, ParsesGroupWithCardinality)
{
    std::string source = "<expr> ::= (<term> | <factor>)* ;";
    Parser parser(source);
    SyntaxTree grammar = parser.parse_all();

    ASSERT_EQ(grammar.rules.size(), 1);

    auto* part = dynamic_cast<SubPartNode*>(grammar.rules[0].production->content.get());
    ASSERT_NE(part, nullptr);
    ASSERT_EQ(part->element.size(), 1);

    auto* group = dynamic_cast<GroupNode*>(part->element[0].get());
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->card, ElementNode::Cardinality::ZERO_OR_MORE);

    auto* inner_alt = dynamic_cast<AlternatePartsNode*>(group->inner->content.get());
    ASSERT_NE(inner_alt, nullptr);
    ASSERT_EQ(inner_alt->subparts.size(), 2);
}

TEST(EBNFParserUnit, ParsesSequenceOfSymbols)
{
    std::string source = "<pair> ::= <key> <value> ;";
    Parser parser(source);
    SyntaxTree grammar = parser.parse_all();

    ASSERT_EQ(grammar.rules.size(), 1);

    auto* sub = dynamic_cast<SubPartNode*>(grammar.rules[0].production->content.get());
    ASSERT_NE(sub, nullptr);
    ASSERT_EQ(sub->element.size(), 2);

    EXPECT_EQ(dynamic_cast<SymbolNode*>(sub->element[0].get())->token.lexeme, "key");
    EXPECT_EQ(dynamic_cast<SymbolNode*>(sub->element[1].get())->token.lexeme, "value");
}

TEST(EBNFParserUnit, ParsesSymbolModifiers)
{
    std::string source = "<expr> ::= <term>* <factor>? <digit>+ ;";
    Parser parser(source);
    SyntaxTree grammar = parser.parse_all();

    auto* sub = dynamic_cast<SubPartNode*>(grammar.rules[0].production->content.get());
    ASSERT_NE(sub, nullptr);
    ASSERT_EQ(sub->element.size(), 3);

    EXPECT_EQ(sub->element[0]->card, ElementNode::Cardinality::ZERO_OR_MORE);
    EXPECT_EQ(sub->element[1]->card, ElementNode::Cardinality::OPTIONAL);
    EXPECT_EQ(sub->element[2]->card, ElementNode::Cardinality::ONE_OR_MORE);
}

TEST(EBNFParserError, MissingSemicolon)
{
    std::string source = "<expr> ::= <term>";
    Parser parser(source);

    try {
        parser.parse_all();
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error& e) {
        const char* expected = "FileName:1:18: Parse error: missing operator (expected ';', got '')";
        EXPECT_STREQ(expected, e.what());
    }
    catch (...) {
        FAIL() << "Expected std::runtime_error, but caught a different exception type";
    }
}

TEST(EBNFParserError, UnexpectedToken)
{
    std::string source = "<expr> <term> ;"; // Missing ::=
    Parser parser(source);

    try {
        parser.parse_all();
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error& e) {
        const char* expected = "FileName:1:8: Parse error: missing operator (expected '::=', got 'term')";
        EXPECT_STREQ(expected, e.what());
    }
    catch (...) {
        FAIL() << "Expected std::runtime_error, but caught a different exception type";
    }
}

TEST(EBNFParserIntegration, ParsesMiniGrammar)
{
    std::string source = R"abc(
        <expr> ::= <term> | <term> OPERATOR("+") <expr> ;
        <term> ::= <factor> | <factor> OPERATOR("*") <term> ;
        <factor> ::= <number> | PUNCTIATION("(") <expr> PUNCTIATION(")") ;
    )abc";

    Parser parser(source);
    SyntaxTree grammar = parser.parse_all();

    ASSERT_EQ(grammar.rules.size(), 3);
    EXPECT_EQ(grammar.rules[0].name, "expr");
    EXPECT_EQ(grammar.rules[1].name, "term");
    EXPECT_EQ(grammar.rules[2].name, "factor");
}

TEST(EBNFParserIntegration, ParsesChiComGrammar)
{
    std::filesystem::path file_name( "../../../chicom8/src/chicom8.bnf");
    Parser parser( file_name );

    try {
        SyntaxTree grammar = parser.parse_all();
    }
    catch(...) {
        FAIL() << "Did not expect to throw an exception parsing 'chicom8.bnf'";
    }
}
