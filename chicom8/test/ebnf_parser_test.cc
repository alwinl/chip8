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

void PrintTo( const Grammar& grammar, std::ostream* os )
{
    *os << "\n" << grammar;
}

// Helper to tokenize a string (not from file)
std::vector<Token> tokenize_string( const char * src )
{
    return Tokeniser( std::string(src) ).tokenise_all();
}

TEST(EBNFParserTest, ParsesSingleRule)
{
    auto tokens = tokenize_string("<expr> ::= \"42\" ;");
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "expr",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::STRING_LITERAL, "\"42\"", 1, 12 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST(EBNFParserTest, ParsesAlternatives)
{
    auto tokens = tokenize_string("<expr> ::= \"42\" | <ident> ;");
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "expr",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::STRING_LITERAL, "\"42\"", 1, 12 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                },
                Part {
                    Symbol {
                        .token = { Token::Type::NONTERMINAL, "ident", 1, 19 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST(EBNFParserTest, ParsesMultipleRules)
{
    auto tokens = tokenize_string("<expr> ::= \"42\" ;\n<ident> ::= \"x\" ;");
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "expr",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::STRING_LITERAL, "\"42\"", 1, 12 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        },
        Rule {
            .name = "ident",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::STRING_LITERAL, "\"x\"", 2, 13 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST(EBNFParserTest, HandlesStringLiteralsAndNonterminals)
{
    auto tokens = tokenize_string("<rule> ::= \"foo\" <bar> ;");
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "rule",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::STRING_LITERAL, "\"foo\"", 1, 12 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    },
                    Symbol {
                        .token = { Token::Type::NONTERMINAL, "bar", 1, 18 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST(EBNFParserTest, HandlesInvalidInputGracefully)
{
    auto tokens = tokenize_string("<expr> \"42\" ;");    // Missing ::= 
    
    // Should throw exception
    try {
        auto rules = Parser(tokens).syntax_tree();
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error&) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST( EBNFParserTest, ParseRegexRule )
{
    auto tokens = tokenize_string( "<identifier> ::= /[a-zA-Z][0-9a-zA-Z_]*/ ;" );
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "identifier",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::REGEX, "/[a-zA-Z][0-9a-zA-Z_]*/", 1, 18 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST( EBNFParserTest, ParseTwoParts )
{
    auto tokens = tokenize_string( "<number> ::= <decimal> | <hex> ; ");
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "number",
            .production = Production {
                Part {
                    Symbol {
                        .token = { Token::Type::NONTERMINAL, "decimal", 1, 14 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    },
                },
                Part {
                    Symbol {
                        .token = { Token::Type::NONTERMINAL, "hex", 1, 26 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    }
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST( EBNFParserTest, ParseGroup )
{
    auto tokens = tokenize_string( "<unary> ::= (\"!\" | \"-\") <unary> | <primary> ;");
    auto actual = Parser(tokens).syntax_tree();

    Grammar expected = {
        Rule {
            .name = "unary",
            .production = Production {
                Part {
                    Symbol {
                        .symbol_group = {
                            Symbol {
                                .token = { Token::Type::STRING_LITERAL, "\"!\"", 1, 14 },
                                .optional = false,
                                .repeated = false,
                                .symbol_group = {}
                            },
                            Symbol {
                                .token = { Token::Type::STRING_LITERAL, "\"-\"", 1, 20 },
                                .optional = false,
                                .repeated = false,
                                .symbol_group = {}
                            }
                        }
                    },
                    Symbol {
                        .token = { Token::Type::NONTERMINAL, "unary", 1, 25 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    },
                },
                Part {
                    Symbol {
                        .token = { Token::Type::NONTERMINAL, "primary", 1, 35 },
                        .optional = false,
                        .repeated = false,
                        .symbol_group = {}
                    },
                },
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

