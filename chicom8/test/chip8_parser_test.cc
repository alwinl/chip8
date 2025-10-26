/*
 * parser_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "parser.h"

void PrintTo( const Program& program, std::ostream* os )
{
    *os << "\n" << program;
}

class TestableParser : public Parser {
public:
    TestableParser( const Tokens& tokens ) : Parser(tokens) {
        cursor = tokens.begin();
    }

    // Expose protected members for testing
    using Parser::consume;
    using Parser::match;
    using Parser::peek;
    using Parser::forward_cursor;
};

// Fixture for common parser setup.
class ParserTest : public ::testing::Test {
protected:
    Tokens tokens;
    std::unique_ptr<TestableParser> parser;

    void SetUpParser(const std::string& source) {
        tokens = Tokeniser( source ).tokenise_all();
        parser = std::make_unique<TestableParser>(tokens);
    }
};

//
// ---- TESTS BEGIN ----
//

TEST_F(ParserTest, Consume_TypeOnly_SucceedsOnMatch)
{
    SetUpParser("var");

    Token tok = parser->consume(Token::Type::KEYWORD, "Expected keyword");
    EXPECT_EQ(tok.lexeme, "var");
    EXPECT_EQ(tok.type, Token::Type::KEYWORD);
}

TEST_F(ParserTest, Consume_TypeOnly_ThrowsOnMismatch)
{
    SetUpParser("123");

    EXPECT_THROW(
        parser->consume(Token::Type::IDENTIFIER, "Expected identifier"),
        std::runtime_error
    );
}

TEST_F(ParserTest, Consume_TypeAndLexeme_Succeeds)
{
    SetUpParser(";");

    Token tok = parser->consume(Token::Type::PUNCTUATION, ";", "Expected semicolon");
    EXPECT_EQ(tok.lexeme, ";");
}

TEST_F(ParserTest, Consume_TypeAndLexeme_ThrowsOnWrongLexeme)
{
    SetUpParser("{");

    EXPECT_THROW(
        parser->consume(Token::Type::PUNCTUATION, ";", "Expected semicolon"),
        std::runtime_error
    );
}

TEST_F(ParserTest, Consume_TypeAndLexeme_ThrowsOnWrongType)
{
    SetUpParser("42");

    EXPECT_THROW(
        parser->consume(Token::Type::PUNCTUATION, ";", "Expected semicolon"),
        std::runtime_error
    );
}

//
// Integration-level test: simple variable declaration
//
TEST_F(ParserTest, ParseVarDecl_Simple)
{
    SetUpParser("var counter : nibble;");

    Program program = parser->AST();

    Program expected {
        .declarations {
            VarDecl {
                .identifier = "counter",
                .type {
                    .type = Type::eTypeTypes::NIBBLE,
                    .sprite_size = 0
                }
            }
        }
    };

    EXPECT_EQ( program, expected );
}

//
// Integration-level test: multiple declarations
//
TEST_F(ParserTest, ParseMultipleDecls)
{
    SetUpParser(
R"(
    var x : byte;
    var y : snack;
)"
    );

    Program actual = parser->AST();

    Program expected {
        .declarations {
            VarDecl {
                .identifier = "x",
                .type {
                    .type = Type::eTypeTypes::BYTE,
                    .sprite_size = 0
                }
            },
            VarDecl {
                .identifier = "y",
                .type {
                    .type = Type::eTypeTypes::SNACK,
                    .sprite_size = 0
                }
            }
        }
    };

    EXPECT_EQ( actual, expected );
}

//
// Integration-level test: bad declaration throws
//
TEST_F(ParserTest, ParseVarDecl_MissingSemicolonThrows)
{
    SetUpParser("var counter : number");

    EXPECT_THROW(parser->AST(), std::runtime_error);
}

TEST_F(ParserTest, ParseFuncDecl_Simple)
{
    SetUpParser("func foo() {}");

    Program actual = parser->AST();

    Program expected {
        .declarations {
            FuncDecl {
                "foo",
                ParamList {},
                Vardecls {},
                Stmts {}
            },
        }
    };

    EXPECT_EQ( actual, expected );
}

TEST_F(ParserTest, ParseFuncDecl_WithParams)
{
    SetUpParser("func add(a: number, b: sprite 5 ) {}");

    Program actual = parser->AST();

    Program expected {
        .declarations {
            FuncDecl {
                "add",
                ParamList {
                    TypedIdentifier {
                        "a",
                        { .type = Type::eTypeTypes::NUMBER, .sprite_size = 0 }
                    },
                    TypedIdentifier {
                        "b",
                        { .type = Type::eTypeTypes::SPRITE, .sprite_size = 5 }
                    }
                },
                Vardecls {},
                Stmts {}
            }
        }
    };

    EXPECT_EQ(actual, expected);
}

// TEST_F(ParserTest, ParseFuncDecl_WithBody)
// {
//     SetUpParser(R"(func foo() {
//     var x: number;
//     x = 42;
// })");

//     Program actual = parser->AST();

//     Program expected {
//         .declarations {
//             FuncDecl {
//                 "foo",
//                 ParamList {},
//                 Vardecls {},
//                 Stmts
//                  {
//                     VarDecl {
//                         "x",
//                         Type::eTypeTypes::NUMBER
//                     },
//                     // ExprStmt {
//                     //     .expr = Expr {
//                     //         .lhs = {
//                     //             .type = Token::Type::IDENTIFIER,
//                     //             .lexeme = "x",
//                     //             .line = 3,
//                     //             .column = 5
//                     //         },
//                     //         .op = "=",
//                     //         .rhs = Expr {
//                     //             .type = Token::Type::NUMBER,
//                     //             .lexeme = "42",
//                     //             .line = 3,
//                     //             .column = 9
//                     //         }
//                     //     }
//                     // }
//                 }
//             }
//         }
//     };

//     EXPECT_EQ(actual, expected);
// }

TEST_F(ParserTest, ParseFuncDecl_Error_MissingBrace)
{
    SetUpParser("func foo() ");

    EXPECT_THROW({
        parser->AST();
    }, std::runtime_error);
}
