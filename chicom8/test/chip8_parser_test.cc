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
    TestableParser() : Parser() {}

    // Expose protected members for testing
    using Parser::consume;
    using Parser::match;
    using Parser::peek;
    using Parser::forward_cursor;
	using Parser::cursor;
	using Parser::tokens;
};

// Fixture for common parser setup.
class ParserTest : public ::testing::Test {
protected:
    // Tokens tokens;
    std::unique_ptr<TestableParser> parser;

    void SetUpParser(const std::string& source)
	{
        parser = std::make_unique<TestableParser>();

		auto tokens = Tokeniser( source ).tokenise_all();
		parser->tokens = tokens;
		parser->cursor = tokens.begin();
    }

	void SetUpParser()
	{
        parser = std::make_unique<TestableParser>();
	}
};

//
// ---- TESTS BEGIN ----
//

// TEST_F(ParserTest, Consume_TypeOnly_SucceedsOnMatch)
// {
//     SetUpParser("var");

//     Token tok = parser->consume(Token::Type::KEYWORD, "Expected keyword");
//     EXPECT_EQ(tok.lexeme, "var");
//     EXPECT_EQ(tok.type, Token::Type::KEYWORD);
// }

// TEST_F(ParserTest, Consume_TypeOnly_ThrowsOnMismatch)
// {
//     SetUpParser("123");

//     EXPECT_THROW(
//         parser->consume(Token::Type::IDENTIFIER, "Expected identifier"),
//         std::runtime_error
//     );
// }

// TEST_F(ParserTest, Consume_TypeAndLexeme_Succeeds)
// {
//     SetUpParser(";");

//     Token tok = parser->consume(Token::Type::PUNCTUATION, ";", "Expected semicolon");
//     EXPECT_EQ(tok.lexeme, ";");
// }

// TEST_F(ParserTest, Consume_TypeAndLexeme_ThrowsOnWrongLexeme)
// {
//     SetUpParser("{");

//     EXPECT_THROW(
//         parser->consume(Token::Type::PUNCTUATION, ";", "Expected semicolon"),
//         std::runtime_error
//     );
// }

// TEST_F(ParserTest, Consume_TypeAndLexeme_ThrowsOnWrongType)
// {
//     SetUpParser("42");

//     EXPECT_THROW(
//         parser->consume(Token::Type::PUNCTUATION, ";", "Expected semicolon"),
//         std::runtime_error
//     );
// }

//
// Integration-level test: simple variable declaration
//
TEST_F(ParserTest, ParseVarDecl_Simple)
{
	TestableParser parser = TestableParser();

	std::string input("var counter : nibble;");

    Program program = parser.make_AST( input );

    Program expected;

    expected.declarations.push_back( std::make_unique<VarDecl> ( std::make_unique<Identifier>("counter"), VarDecl::eTypes::NIBBLE ) );

    EXPECT_EQ( program, expected );
}

//
// Integration-level test: multiple declarations
//
TEST_F(ParserTest, ParseMultipleDecls)
{
	std::string input(
R"(
    var x : byte;
    var y : snack;
)"
    );

    SetUpParser();
    Program actual = parser->make_AST( input );

    Program expected;

    expected.declarations.push_back( std::make_unique<VarDecl>( std::make_unique<Identifier>("x"), VarDecl::eTypes::BYTE ) );
    expected.declarations.push_back( std::make_unique<VarDecl>( std::make_unique<Identifier>("y"), VarDecl::eTypes::SNACK ) );

    EXPECT_EQ( actual, expected );
}

//
// Integration-level test: bad declaration throws
//
TEST_F(ParserTest, ParseVarDecl_MissingSemicolonThrows)
{
	std::string input( "var counter : number" );
    SetUpParser();

    EXPECT_THROW(parser->make_AST( input ), std::runtime_error);
}

TEST_F(ParserTest, ParseFuncDecl_Simple)
{
	std::string input( "func foo() {}" );
    SetUpParser();

    Program actual = parser->make_AST( input );

    Program expected;

    std::vector<std::unique_ptr<VarDecl>> paramlist;
    std::vector<std::unique_ptr<VarDecl>> vardecls;
    std::vector<std::unique_ptr<Stmt>> stmt_list;

    expected.declarations.push_back( std::make_unique<FuncDecl>( std::make_unique<Identifier>("foo"), std::move( paramlist ), std::move( vardecls ), std::move( stmt_list ) ) );

    std::cout << "actual " << actual << '\n';
    std::cout << "expected " << expected << '\n';

    EXPECT_EQ( actual, expected );
}

TEST_F(ParserTest, ParseFuncDecl_WithParams)
{
	std::string input("func add(a: number, b: sprite 5 ) {}");

    SetUpParser();

    Program actual = parser->make_AST( input );

    Program expected;

    std::vector<std::unique_ptr<VarDecl>> paramlist;
    std::vector<std::unique_ptr<VarDecl>> vardecls;
    std::vector<std::unique_ptr<Stmt>> stmt_list;

    paramlist.push_back( std::make_unique<VarDecl>( std::make_unique<Identifier>("a"), VarDecl::eTypes::NUMBER ) );
    paramlist.push_back( std::make_unique<VarDecl>( std::make_unique<Identifier>("b"), VarDecl::eTypes::SPRITE, std::make_unique<Number>(5) ) );

    expected.declarations.push_back( std::make_unique<FuncDecl>(std::make_unique<Identifier>("add"), std::move(paramlist), std::move( vardecls ), std::move( stmt_list ) ) );

    EXPECT_EQ(actual, expected);
}

TEST_F(ParserTest, ParseFuncDecl_WithBody)
{
	std::string input(
R"(func foo() {
    var x: bool;
    x = keydown KEY_1;
})"
	);
    SetUpParser();

    Program actual = parser->make_AST( input );

    Program expected;

    std::vector<std::unique_ptr<VarDecl>> paramlist;
    std::vector<std::unique_ptr<VarDecl>> vardecls;
    std::vector<std::unique_ptr<Stmt>> stmt_list;

    vardecls.push_back( std::make_unique<VarDecl> ( std::make_unique<Identifier>("x"), VarDecl::eTypes::BOOL ) );

    stmt_list.push_back(
        std::make_unique<ExprStmt>(
            std::make_unique<BinaryExpr>(
                std::make_unique<Identifier>("x"),
                BinaryExpr::Operator::ASSIGN,
                std::make_unique<KeyDownExpr>( std::make_unique<Identifier>("KEY_1") )
            )
        )
    );

    expected.declarations.push_back( std::make_unique<FuncDecl>(std::make_unique<Identifier>("foo"), std::move(paramlist), std::move( vardecls ), std::move( stmt_list ) ) );

    EXPECT_EQ(actual, expected);
}

TEST_F(ParserTest, ParseFuncDecl_Error_MissingBrace)
{
	std::string input("func foo() ");
    SetUpParser();

    EXPECT_THROW({ parser->make_AST( input ); }, std::runtime_error);
}

TEST_F(ParserTest, ParseFullProgram)
{
    std::string source_code = R"(
        var flag : bool;
        var word : byte;
        var address : snack;
        var image : sprite 5;

        func foo( x : number, y : number, img : sprite 6 )
        {
            var internal : key;
            var second : number;

            if( x == 5 )
                keydown KEY_1;

            while( getkey ) {
                internal = ( y + 6 ) * 3;
                draw( img, x + 10, y + 10 );
                second = rnd 2*8;
            }
            flag = false;
        }

        func main()
        {
            foo( X, Y, IMG );
            return 9;
        }
    )";

    SetUpParser();

    // 1. Parse program
    Program actual = parser->make_AST( source_code );

    // 2. Build expected Program
    Program expected;

    // Global variables
    expected.declarations.push_back(std::make_unique<VarDecl>(std::make_unique<Identifier>("flag"), VarDecl::eTypes::BOOL));
    expected.declarations.push_back(std::make_unique<VarDecl>(std::make_unique<Identifier>("word"), VarDecl::eTypes::BYTE));
    expected.declarations.push_back(std::make_unique<VarDecl>(std::make_unique<Identifier>("address"), VarDecl::eTypes::SNACK));
    expected.declarations.push_back(std::make_unique<VarDecl>(
        std::make_unique<Identifier>("image"), VarDecl::eTypes::SPRITE, std::make_unique<Number>(5)));

    // Function foo
    std::vector<std::unique_ptr<VarDecl>> foo_params;
    foo_params.push_back(make_unique<VarDecl>( std::make_unique<Identifier>("x"), VarDecl::eTypes::NUMBER ));
    foo_params.push_back(make_unique<VarDecl>( std::make_unique<Identifier>("y"), VarDecl::eTypes::NUMBER ));
    foo_params.push_back(make_unique<VarDecl>( std::make_unique<Identifier>("img"), VarDecl::eTypes::SPRITE, std::make_unique<Number>(6) ));

    std::vector<std::unique_ptr<VarDecl>> foo_vars;
    foo_vars.push_back(std::make_unique<VarDecl>(std::make_unique<Identifier>("internal"), VarDecl::eTypes::KEY));
    foo_vars.push_back(std::make_unique<VarDecl>(std::make_unique<Identifier>("second"), VarDecl::eTypes::NUMBER));

    std::vector<std::unique_ptr<Stmt>> foo_body;

    // if (x == 5) keydown KEY_1;
    auto if_stmt = std::make_unique<IfStmt>(
        std::make_unique<BinaryExpr>(
            std::make_unique<Identifier>("x"),
            BinaryExpr::Operator::EQUALS,
            std::make_unique<Number>(5)),
        std::make_unique<ExprStmt>(
            std::make_unique<KeyDownExpr>(
                std::make_unique<Identifier>("KEY_1"))),
        nullptr
    );
    foo_body.push_back(std::move(if_stmt));

    // while(getkey) { ... }
    std::vector<std::unique_ptr<Stmt>> while_body;

    // internal = (y + 6) * 3;
    while_body.push_back(std::make_unique<ExprStmt>(
        std::make_unique<BinaryExpr>(
            std::make_unique<Identifier>("internal"),
            BinaryExpr::Operator::ASSIGN,
            std::make_unique<BinaryExpr>(
                std::make_unique<BracedExpr>(
                    std::make_unique<BinaryExpr>(
                        std::make_unique<Identifier>("y"),
                        BinaryExpr::Operator::ADD,
                        std::make_unique<Number>(6))
                ),
                BinaryExpr::Operator::MULTIPLY,
                std::make_unique<Number>(3)
            )
        )
    ));

    // draw img, x + 10, y + 10;
    while_body.push_back(std::make_unique<DrawStmt>(
        std::make_unique<Identifier>("img"),
        std::make_unique<BinaryExpr>(
            std::make_unique<Identifier>("x"),
            BinaryExpr::Operator::ADD,
            std::make_unique<Number>(10)
        ),
        std::make_unique<BinaryExpr>(
            std::make_unique<Identifier>("y"),
            BinaryExpr::Operator::ADD,
            std::make_unique<Number>(10)
        ),
        nullptr
    ));

    // second = rnd 2*8;
    while_body.push_back(std::make_unique<ExprStmt>(
        std::make_unique<BinaryExpr>(
            std::make_unique<Identifier>("second"),
            BinaryExpr::Operator::ASSIGN,
            std::make_unique<RndExpr>(
                std::make_unique<BinaryExpr>(
                    std::make_unique<Number>(2),
                    BinaryExpr::Operator::MULTIPLY,
                    std::make_unique<Number>(8)
                )
            )
        )
    ));

    auto while_stmt = std::make_unique<WhileStmt>(
        std::make_unique<GetKeyExpr>(),
        std::make_unique<BlockStmt>(std::move(while_body))
    );
    foo_body.push_back(std::move(while_stmt));

    // flag = false;
    foo_body.push_back(std::make_unique<ExprStmt>(
        std::make_unique<BinaryExpr>(
            std::make_unique<Identifier>("flag"),
            BinaryExpr::Operator::ASSIGN,
            std::make_unique<Bool>(false)
        )
    ));

    expected.declarations.push_back(std::make_unique<FuncDecl>(
        std::make_unique<Identifier>("foo"),
        std::move(foo_params),
        std::move(foo_vars),
        std::move(foo_body)
    ));

    // Function main
    std::vector<std::unique_ptr<VarDecl>> main_params;
    std::vector<std::unique_ptr<VarDecl>> main_vars;
    std::vector<std::unique_ptr<Stmt>> main_body;

    std::vector<std::unique_ptr<Expr>> args;
    args.push_back( std::make_unique<Identifier>("X") );
    args.push_back( std::make_unique<Identifier>("Y") );
    args.push_back( std::make_unique<Identifier>("IMG") );

    main_body.push_back(std::make_unique<ExprStmt>(
        std::make_unique<FuncCallExpr>(
            std::make_unique<Identifier>("foo"),
            std::move( args)
        )
    ));

    main_body.push_back(std::make_unique<ReturnStmt>(
        std::make_unique<Number>(9)
    ));

    expected.declarations.push_back(std::make_unique<FuncDecl>(
        std::make_unique<Identifier>("main"),
        std::move(main_params),
        std::move(main_vars),
        std::move(main_body)
    ));

    // 3. Compare
    EXPECT_EQ(actual, expected);
}
