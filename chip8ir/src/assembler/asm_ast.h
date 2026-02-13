/*
 * asm_ast.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <memory>

class ASTExpression;

struct IdentifierExpr
{
    std::string text;
};

struct NumberExpr
{
    uint32_t value;
};

struct ASTBinaryExpr
{
	char op;                       // '+', '-', '*', '/'
    std::unique_ptr<ASTExpression> lhs;
    std::unique_ptr<ASTExpression> rhs;
};

using ExpressionType = std::variant<NumberExpr, IdentifierExpr, ASTBinaryExpr>;

struct ASTExpression
{
	ExpressionType expression;
	int column;
};

using ASTExpressions = std::vector<ASTExpression>;

struct ASTLabel
{
	std::string name;
	int column;
};

struct ASTInstruction
{
	std::string mnemonic;
	int column;
    ASTExpressions operands;
};

struct ASTDirective
{
    std::string name;
	int column;
    ASTExpressions args;
};

struct ASTEqu
{
	std::string name;
	int column;
	ASTExpression value;
};

using LabelType = std::optional<ASTLabel>;
using BodyType = std::variant< ASTInstruction, ASTDirective, ASTEqu >;

struct ASTElement
{
	int line;
    LabelType label;
    BodyType body;
};

using ASTProgram = std::vector<ASTElement>;
