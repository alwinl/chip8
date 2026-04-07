/*
 * expression_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/ast.h"
#include "assembler/symbol_table.h"
#include "assembler/assembler.h"
#include "assembler/expression.h"


class ExpressionTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}
};

TEST_F(ExpressionTest, Literal)
{
    ASMSymbolTable symbols;

    ASTExpression expr{ ASTNumberExpr{42}, 0 };

    EXPECT_EQ(evaluate_expression(expr, symbols), 42);
}

TEST_F(ExpressionTest, Identifier)
{
    ASMSymbolTable symbols;
    symbols.define_constant("FOO", 123);

    ASTExpression expr{ ASTIdentifierExpr{"FOO"}, 0 };

    EXPECT_EQ(evaluate_expression(expr, symbols), 123);
}

TEST_F(ExpressionTest, BinaryAdd)
{
    ASMSymbolTable symbols;

    ASTExpression lhs{ ASTNumberExpr{10}, 0 };
    ASTExpression rhs{ ASTNumberExpr{5}, 0 };

    ASTBinaryExpr bin{
        '+',
        std::make_unique<ASTExpression>(std::move(lhs)),
        std::make_unique<ASTExpression>(std::move(rhs))
    };

    ASTExpression expr{ std::move(bin), 0 };

    EXPECT_EQ(evaluate_expression(expr, symbols), 15);
}

TEST_F(ExpressionTest, InvalidRegister)
{
	ASTExpression expr{ ASTIdentifierExpr{"V99"}, 0 };

    EXPECT_THROW(parse_reg(expr), std::runtime_error);
}

TEST_F(ExpressionTest, DivisionByZero)
{
    ASMSymbolTable symbols;

    ASTExpression lhs{ ASTNumberExpr{10}, 0 };
    ASTExpression rhs{ ASTNumberExpr{0}, 0 };

    ASTBinaryExpr bin{
        '/',
        std::make_unique<ASTExpression>(std::move(lhs)),
        std::make_unique<ASTExpression>(std::move(rhs))
    };

    ASTExpression expr{ std::move(bin), 0 };

    EXPECT_EQ(evaluate_expression(expr, symbols), 0);
}