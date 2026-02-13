/*
 * asm_parser_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/parser.h"

class ASMParserTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}

	ASMParser parser;
	ASMTokens tokens;
};

TEST_F(ASMParserTest, NoTokens)
{
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	EXPECT_EQ( program.size(), 0 );
}

TEST_F(ASMParserTest, SingleLabel)
{
	tokens.push_back( ASMToken { ASMToken::Type::LABEL, "start", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	EXPECT_TRUE( program[0].label.has_value() );
	EXPECT_EQ( program[0].label.value().name, "start" );
}

TEST_F(ASMParserTest, SingleMnemonic)
{
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "CLS", 1, 4 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	auto instr_ptr = std::get_if<ASTInstruction>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->mnemonic, "CLS" );
}

TEST_F(ASMParserTest, SingleInstructionWithArgument)
{
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "JP", 1, 4 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "0x300", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	auto instr_ptr = std::get_if<ASTInstruction>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->mnemonic, "JP" );

	ASSERT_EQ( instr_ptr->operands.size(), 1 );

	auto operand_ptr = std::get_if<NumberExpr>(&instr_ptr->operands[0].expression);
	ASSERT_NE(operand_ptr, nullptr);

	EXPECT_EQ( operand_ptr->value, 0x300 );
}

TEST_F(ASMParserTest, SingleInstructionWithArguments)
{
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "LD", 1, 4 } );
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "V0", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::COMMA, ",", 1, 9 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "10", 1, 11 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	EXPECT_EQ( program[0].line, 1 );

	auto instr_ptr = std::get_if<ASTInstruction>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->mnemonic, "LD" );

	ASSERT_EQ( instr_ptr->operands.size(), 2 );

	auto operand1_ptr = std::get_if<IdentifierExpr>(&instr_ptr->operands[0].expression);
	ASSERT_NE(operand1_ptr, nullptr);
	EXPECT_EQ( operand1_ptr->text, "V0" );

	auto operand2_ptr = std::get_if<NumberExpr>(&instr_ptr->operands[1].expression);
	ASSERT_NE(operand2_ptr, nullptr);
	EXPECT_EQ( operand2_ptr->value, 10 );
}

TEST_F(ASMParserTest, SingleInstructionWithLabelAndArguments)
{
	tokens.push_back( ASMToken { ASMToken::Type::LABEL, "start", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "LD", 1, 4 } );
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "V0", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::COMMA, ",", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "10", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	EXPECT_TRUE( program[0].label.has_value() );
	EXPECT_EQ( program[0].label.value().name, "start" );

	EXPECT_EQ( program[0].line, 1 );

	auto instr_ptr = std::get_if<ASTInstruction>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->mnemonic, "LD" );

	ASSERT_EQ( instr_ptr->operands.size(), 2 );

	auto operand1_ptr = std::get_if<IdentifierExpr>(&instr_ptr->operands[0].expression);
	ASSERT_NE(operand1_ptr, nullptr);
	EXPECT_EQ( operand1_ptr->text, "V0" );

	auto operand2_ptr = std::get_if<NumberExpr>(&instr_ptr->operands[1].expression);
	ASSERT_NE(operand2_ptr, nullptr);
	EXPECT_EQ( operand2_ptr->value, 10 );
}

TEST_F(ASMParserTest, Directive)
{
	tokens.push_back( ASMToken { ASMToken::Type::DIRECTIVE, ".DW", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	auto instr_ptr = std::get_if<ASTDirective>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->name, ".DW" );
	EXPECT_TRUE( instr_ptr->args.empty() );
}

TEST_F(ASMParserTest, DirectiveWithLabel)
{
	tokens.push_back( ASMToken { ASMToken::Type::LABEL, "start", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::DIRECTIVE, ".DW", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	EXPECT_TRUE( program[0].label.has_value() );
	EXPECT_EQ( program[0].label.value().name, "start" );

	auto instr_ptr = std::get_if<ASTDirective>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->name, ".DW" );
	EXPECT_TRUE( instr_ptr->args.empty() );
}

TEST_F(ASMParserTest, DirectiveWithLabelAndArgs)
{
	tokens.push_back( ASMToken { ASMToken::Type::LABEL, "start", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::DIRECTIVE, ".DW", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "0x10", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::COMMA, ",", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "0x20", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::COMMA, ",", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "0x30", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	EXPECT_TRUE( program[0].label.has_value() );
	EXPECT_EQ( program[0].label.value().name, "start" );

	auto instr_ptr = std::get_if<ASTDirective>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->name, ".DW" );
	EXPECT_EQ( instr_ptr->args.size(), 3 );
}

TEST_F(ASMParserTest, VariableDef)
{
	tokens.push_back( ASMToken { ASMToken::Type::IDENTIFIER, "X_POS", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::ASSIGNMENT, "EQU", 1, 1 } );
	tokens.push_back( ASMToken { ASMToken::Type::NUMBER, "0x10", 1, 7 } );
	tokens.push_back( ASMToken { ASMToken::Type::END_OF_INPUT, "", 0, 0 } );

	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 1 );

	EXPECT_FALSE( program[0].label.has_value() );

	auto instr_ptr = std::get_if<ASTEqu>(&program[0].body);
	ASSERT_NE(instr_ptr, nullptr);

	EXPECT_EQ( instr_ptr->name, "X_POS" );

	auto expr_ptr = std::get_if<NumberExpr>(&instr_ptr->value.expression);
	ASSERT_NE(expr_ptr, nullptr);
	EXPECT_EQ( expr_ptr->value, 0x10 );
}
