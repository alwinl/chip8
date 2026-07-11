/*
 * chip8_ostream_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <memory>
#include <vector>

#include "chip8_ast.h"

TEST(OStreamTest, NumberPrintsName)
{
    Number id(42);
    std::ostringstream os;
    os << id;
    EXPECT_EQ(os.str(), "42");
}

TEST(OStreamTest, BoolPrintsName)
{
    Bool id( true );
    std::ostringstream os;
    os << id;
    EXPECT_EQ(os.str(), "true");
}

TEST(OStreamTest, IdentifierPrintsName)
{
    Identifier id("foo");
    std::ostringstream os;
    os << id;
    EXPECT_EQ(os.str(), "foo");
}

TEST(OStreamTest, PrintsIdentifierExpr)
{
    Identifier id("x");
    std::ostringstream os;
    os << static_cast<const Expr&>(id);
    EXPECT_EQ(os.str(), "Identifier(x)");
}

TEST(OStreamTest, PrintsBinaryExpr)
{
    auto lhs = std::make_unique<Identifier>("a");
    auto rhs = std::make_unique<Number>(10);
    BinaryExpr expr( std::move(lhs), BinaryExpr::Operator::ADD, std::move(rhs) );

    std::ostringstream os;
    os << expr;
    EXPECT_EQ(os.str(), "BinaryExpr(Identifier(a) + Number(10))");
}

TEST(OStreamTest, PrintsUnaryExpr)
{
    auto inner = std::make_unique<Identifier>("flag");
    UnaryExpr expr( std::move(inner), UnaryExpr::Operator::NOT );

    std::ostringstream os;
    os << expr;
    EXPECT_EQ(os.str(), "UnaryExpr(!Identifier(flag))");
}

TEST(OStreamTest, PrintsFuncCallExpr)
{
    auto name = std::make_unique<Identifier>("foo");
    std::vector<std::unique_ptr<Expr>> args;
    args.push_back(std::make_unique<Number>(5));
    args.push_back(std::make_unique<Identifier>("x"));

    FuncCallExpr call(std::move(name), std::move(args));

    std::ostringstream os;
    os << call;
    EXPECT_EQ(os.str(), "func foo(Number(5), Identifier(x))");
}

TEST(OStreamTest, PrintsVarDecl)
{
    VarDecl var(std::make_unique<Identifier>("x"), VarDecl::eTypes::BOOL);
    
    std::ostringstream os;
    os << var;
    EXPECT_EQ(os.str(), "x: bool");
}

TEST(OStreamTest, PrintsVarDeclPolymorfic)
{
    std::unique_ptr<Decl> var( std::make_unique<VarDecl>(std::make_unique<Identifier>("x"), VarDecl::eTypes::BOOL) );
    
    std::ostringstream os;
    os << *var;
    EXPECT_EQ(os.str(), "var x: bool");
}

// TEST(OStreamTest, PrintsFuncDecl)
// {
//     auto id = std::make_unique<Identifier>("foo");
//     std::vector<VarDecl> params;
//     std::vector<std::unique_ptr<VarDecl>> vars;
//     vars.push_back(std::make_unique<VarDecl>(std::make_unique<Identifier>("x"), VarDecl::eTypes::BOOL));

//     std::vector<std::unique_ptr<Stmt>> body;
//     body.push_back(std::make_unique<ExprStmt>(std::make_unique<Identifier>("x")));

//     FuncDecl func(std::move(id), std::move(params), std::move(vars), std::move(body));

//     std::ostringstream os;
//     os << func;

//     std::string result = os.str();
//     EXPECT_NE(result.find("FuncDecl"), std::string::npos);
//     EXPECT_NE(result.find("VarDecl( x: bool)"), std::string::npos);
//     EXPECT_NE(result.find("Identifier(x)"), std::string::npos);
// }

TEST(OStreamTest, PrintsProgram)
{
    Program prog;
    auto id = std::make_unique<Identifier>("foo");
    std::vector<std::unique_ptr<VarDecl>> params;
    std::vector<std::unique_ptr<VarDecl>> vars;
    std::vector<std::unique_ptr<Stmt>> body;

    prog.declarations.push_back(
        std::make_unique<FuncDecl>(std::move(id), std::move(params), std::move(vars), std::move(body))
    );

    std::ostringstream os;
    os << prog;

    std::string output = os.str();
    EXPECT_TRUE(output.find("Program: [") != std::string::npos);
    EXPECT_TRUE(output.find("func") != std::string::npos);
}

