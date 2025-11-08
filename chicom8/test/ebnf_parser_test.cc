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
    Rules rules = parser.parse_all().rules;

    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].name, "expr");

    // Production → SubPart → Symbol
    auto* prod = dynamic_cast<SubPart*>(rules[0].production->content.get());
    ASSERT_NE(prod, nullptr);
    ASSERT_EQ(prod->element.size(), 1);

    auto* sym = dynamic_cast<Symbol*>(prod->element[0].get());
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->token.lexeme, "term");
}

TEST(EBNFParserUnit, ParsesAlternation)
{
    std::string source = "<expr> ::= <term> | <factor> ;";
    Parser parser(source);
    Rules rules = parser.parse_all().rules;

    ASSERT_EQ(rules.size(), 1);

    auto* alt = dynamic_cast<AlternateParts*>(rules[0].production->content.get());
    ASSERT_NE(alt, nullptr);
    ASSERT_EQ(alt->subparts.size(), 2);

    auto* left = dynamic_cast<SubPart*>(alt->subparts[0].get());
    auto* right = dynamic_cast<SubPart*>(alt->subparts[1].get());
    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);

    auto* left_symbol = dynamic_cast<Symbol*>(left->element[0].get());
    auto* right_symbol = dynamic_cast<Symbol*>(right->element[0].get());
    ASSERT_NE(left_symbol, nullptr);
    ASSERT_NE(right_symbol, nullptr);

    EXPECT_EQ(left_symbol->token.lexeme, "term");
    EXPECT_EQ(right_symbol->token.lexeme, "factor");
}

TEST(EBNFParserUnit, ParsesGroupWithCardinality)
{
    std::string source = "<expr> ::= (<term> | <factor>)* ;";
    Parser parser(source);
    Rules rules = parser.parse_all().rules;

    ASSERT_EQ(rules.size(), 1);

    auto* part = dynamic_cast<SubPart*>(rules[0].production->content.get());
    ASSERT_NE(part, nullptr);
    ASSERT_EQ(part->element.size(), 1);

    auto* group = dynamic_cast<Group*>(part->element[0].get());
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->card, Cardinality::ZERO_OR_MORE);

    auto* inner_alt = dynamic_cast<AlternateParts*>(group->inner->content.get());
    ASSERT_NE(inner_alt, nullptr);
    ASSERT_EQ(inner_alt->subparts.size(), 2);
}

TEST(EBNFParserUnit, ParsesSequenceOfSymbols)
{
    std::string source = "<pair> ::= <key> <value> ;";
    Parser parser(source);
    Rules rules = parser.parse_all().rules;

    ASSERT_EQ(rules.size(), 1);

    auto* sub = dynamic_cast<SubPart*>(rules[0].production->content.get());
    ASSERT_NE(sub, nullptr);
    ASSERT_EQ(sub->element.size(), 2);

    EXPECT_EQ(dynamic_cast<Symbol*>(sub->element[0].get())->token.lexeme, "key");
    EXPECT_EQ(dynamic_cast<Symbol*>(sub->element[1].get())->token.lexeme, "value");
}

TEST(EBNFParserUnit, ParsesSymbolModifiers)
{
    std::string source = "<expr> ::= <term>* <factor>? <digit>+ ;";
    Parser parser(source);
    Rules rules = parser.parse_all().rules;

    auto* sub = dynamic_cast<SubPart*>(rules[0].production->content.get());
    ASSERT_NE(sub, nullptr);
    ASSERT_EQ(sub->element.size(), 3);

    EXPECT_EQ(sub->element[0]->card, Cardinality::ZERO_OR_MORE);
    EXPECT_EQ(sub->element[1]->card, Cardinality::OPTIONAL);
    EXPECT_EQ(sub->element[2]->card, Cardinality::ONE_OR_MORE);
}

TEST(EBNFParserError, MissingSemicolon) {
    std::string source = "<expr> ::= <term>";
    Parser parser(source);

    EXPECT_THROW({
        parser.parse_all();
    }, std::runtime_error);
}

TEST(EBNFParserError, UnexpectedToken)
{
    std::string source = "<expr> <term> ;"; // Missing ::=
    Parser parser(source);

    EXPECT_THROW({
        parser.parse_all();
    }, std::runtime_error);
}

TEST(EBNFParserIntegration, ParsesMiniGrammar)
{
    std::string grammar = R"abc(
        <expr> ::= <term> | <term> OPERATOR("+") <expr> ;
        <term> ::= <factor> | <factor> OPERATOR("*") <term> ;
        <factor> ::= <number> | PUNCTIATION("(") <expr> PUNCTIATION(")") ;
    )abc";

    Parser parser(grammar);
    Rules rules = parser.parse_all().rules;

    ASSERT_EQ(rules.size(), 3);
    EXPECT_EQ(rules[0].name, "expr");
    EXPECT_EQ(rules[1].name, "term");
    EXPECT_EQ(rules[2].name, "factor");
}

// void PrintTo( const Grammar& grammar, std::ostream* os )
// {
//     // print_grammar( *os, grammar );
//     *os << grammar;
// }

// // Helper to tokenize a string (not from file)
// std::vector<Token> tokenize_string( const char * src )
// {
//     return Tokeniser( std::string(src) ).tokenise_all();
// }

// TEST(EBNFParserTest, ParsesSingleRule)
// {
//     auto tokens = tokenize_string("<expr> ::= \"42\" ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["expr"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"42\"", 1, 12 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, ParsesAlternatives)
// {
//     auto tokens = tokenize_string("<expr> ::= \"42\" | <ident> ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["expr"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"42\"", 1, 12 }
//         )
//     );
//     expected.rules["expr"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "ident", 1, 19 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, ParsesMultipleRules)
// {
//     auto tokens = tokenize_string("<expr> ::= \"42\" ;\n<ident> ::= \"x\" ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["expr"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"42\"", 1, 12 }
//         )
//     );
//     expected.rules["ident"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"x\"", 2, 13 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, HandlesStringLiteralsAndNonterminals)
// {
//     auto tokens = tokenize_string("<rule> ::= \"foo\" <bar> ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["rule"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"foo\"", 1, 12 }
//         )
//     );
//     expected.rules["rule"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "bar", 1, 18 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, HandlesInvalidInputGracefully)
// {
//     auto tokens = tokenize_string("<expr> \"42\" ;");    // Missing ::=

//     try {
//         auto rules = Parser(tokens).syntax_tree();
//         FAIL() << "Expected std::runtime_error";
//     } catch (const std::runtime_error&) {
//         SUCCEED();
//     } catch (...) {
//         FAIL() << "Expected std::runtime_error";
//     }
// }

// TEST(EBNFParserTest, ParseRegexRule)
// {
//     auto tokens = tokenize_string("<identifier> ::= /[a-zA-Z][0-9a-zA-Z_]*/ ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::REGEX, "/[a-zA-Z][0-9a-zA-Z_]*/", 1, 18 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, ParseTwoParts)
// {
//     auto tokens = tokenize_string("<number> ::= <decimal> | <hex> ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["number"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "decimal", 1, 14 }
//         )
//     );
//     expected.rules["number"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "hex", 1, 26 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, ParseGroup)
// {
//     auto tokens = tokenize_string("<unary> ::= (\"!\" | \"-\") <unary> | <primary> ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;

//     // First group
//     std::vector<std::unique_ptr<SymbolComponent>> group_children;
//     group_children.push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"!\"", 1, 14 }
//         )
//     );
//     group_children.push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::STRING_LITERAL, "\"-\"", 1, 20 }
//         )
//     );
//     expected.rules["unary"].push_back(
//         std::make_unique<SymbolComposite>(std::move(group_children))
//     );

//     // Unary
//     expected.rules["unary"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "unary", 1, 25 }
//         )
//     );

//     // Primary alternative
//     expected.rules["unary"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "primary", 1, 35 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, ParseTokenProduction)
// {
//     auto tokens = tokenize_string("<typed_identifier> ::= IDENTIFIER PUNCTUATION(\":\") <type> ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["typed_identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::TOKEN_PRODUCTION, "IDENTIFIER", 1, 24 }
//         )
//     );
//     expected.rules["typed_identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::TOKEN_PRODUCTION, "PUNCTUATION(\":\")", 1, 35 }
//         )
//     );
//     expected.rules["typed_identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "type", 1, 52 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST(EBNFParserTest, ParseTokenProductionWithBracket)
// {
//     auto tokens = tokenize_string("<typed_identifier> ::= IDENTIFIER PUNCTUATION(\")\") <type> ;");
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;
//     expected.rules["typed_identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::TOKEN_PRODUCTION, "IDENTIFIER", 1, 24 }
//         )
//     );
//     expected.rules["typed_identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::TOKEN_PRODUCTION, "PUNCTUATION(\")\")", 1, 35 }
//         )
//     );
//     expected.rules["typed_identifier"].push_back(
//         std::make_unique<SymbolLeaf>(
//             Token{ Token::Type::NONTERMINAL, "type", 1, 52 }
//         )
//     );

//     EXPECT_EQ(actual, expected);
// }

// TEST( EBNFParserTest, ParseFunctionDecl )
// {
//     auto tokens = tokenize_string(
//         "<program>         ::= <decl>* ;"
//         "<decl>            ::= <func_decl> | <var_decl> ;"
//         "<func_decl>       ::= KEYWORD(\"func\") IDENTIFIER PUNCTUATION(\"(\") <param_list>? PUNCTUATION(\")\") PUNCTUATION(\"{\") <func_body> PUNCTUATION(\"}\") ;"
//         "<func_body>       ::= ( <var_decl> | <stmt> )*  ;"
//         "<param_list>      ::= <typed_identifier> ( PUNCTUATION(\",\") <typed_identifier> )* ;"
//         "<var_decl>        ::= KEYWORD(\"var\") <typed_identifier> PUNCTUATION(\";\") ;"
//         "<typed_identifier> ::= IDENTIFIER PUNCTUATION(\":\") <type> ;"
//         "<type> ::= TYPE_KEYWORD(\"nibble\")"
//         "        | TYPE_KEYWORD(\"byte\")"
//         "        | TYPE_KEYWORD(\"snack\")"
//         "        | TYPE_KEYWORD(\"number\")"
//         "        | TYPE_KEYWORD(\"bool\")"
//         "        | TYPE_KEYWORD(\"key\")"
//         "        | TYPE_KEYWORD(\"sprite\") <expr>"
//         "        ;"
//         "<stmt>            ::= <expr_stmt>"
//         "                | <if_stmt>"
//         "                | <while_stmt>"
//         "                | <draw_stmt>"
//         "                | <return_stmt>"
//         "                | <block>"
//         "                ;"
//         "<expr_stmt>       ::= <expr> PUNCTUATION(\";\") ;"
//         "<if_stmt>         ::= KEYWORD(\"if\") PUNCTUATION(\"(\") <expr> PUNCTUATION(\")\") <stmt> ( KEYWORD(\"else\") <stmt> )? ;"
//         "<while_stmt>      ::= KEYWORD(\"while\") PUNCTUATION(\"(\") <expr> PUNCTUATION(\")\") <stmt>  ;"
//         "<draw_stmt>       ::= KEYWORD(\"draw\") <identifier> PUNCTUATION(\",\") <expr> PUNCTUATION(\",\") <expr> ( PUNCTUATION(\",\") <expr> )? PUNCTUATION(\";\") ; // draw sprite, x, y [, height]"
//         "<return_stmt>     ::= KEYWORD(\"return\") <expr>? PUNCTUATION(\";\") ;"
//         "<block>           ::= PUNCTUATION(\"{\") <stmt>* PUNCTUATION(\"}\") ;"
//         "<expr>            ::= <identifier> OPERATOR(\"=\") <expr> | <logic_or> ;"
//         "<logic_or>        ::= <logic_and> ( OPERATOR(\"||\") <logic_and> )* ;"
//         "<logic_and>       ::= <equality> ( OPERATOR(\"&&\") <equality> )* ;"
//         "<equality>        ::= <comparison> ((OPERATOR(\"==\") | OPERATOR(\"!=\") ) <comparison>)* ;"
//         "<comparison>      ::= <term> ((OPERATOR(\">\") | OPERATOR(\">=\") | OPERATOR(\"<\") | OPERATOR(\"<=\") ) <term>)* ;"
//         "<term>            ::= <factor> ((OPERATOR(\"+\") | OPERATOR(\"-\") ) <factor>)* ;"
//         "<factor>          ::= <unary> ( (OPERATOR(\"*\") | OPERATOR(\"/\") ) <unary>)* ;"
//         "<unary>           ::= ( OPERATOR(\"!\") | OPERATOR(\"-\") ) <unary> | <primary> ;"
//         "<primary>         ::= <number>"
//         "                | KEYWORD(\"true\")"
//         "                | KEYWORD(\"false\")"
//         "                | KEYWORD(\"bcd\") <identifier>"
//         "                | KEYWORD(\"rnd\") <expr>"
//         "                | KEYWORD(\"keydown\") <identifier>"
//         "                | KEYWORD(\"getkey\")"
//         "                | PUNCTUATION(\"(\") <expr> PUNCTUATION(\")\")"
//         "                | <identifier> ( PUNCTUATION(\"(\") (<expr> (PUNCTUATION(\",\") <expr>)*)? PUNCTUATION(\")\") )?"
//         "                ;"
//         "<identifier>      ::= IDENTIFIER ;"
//         "<number>          ::= NUMBER ;"
//     );
//     auto actual = Parser(tokens).syntax_tree();

//     Grammar expected;

//     EXPECT_EQ( actual, expected );
// }

