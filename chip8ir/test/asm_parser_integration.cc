/*
 * asm_parser_integration.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include <assembler/loader.h>
#include <assembler/tokeniser.h>
#include <assembler/parser.h>

class ASMParserIntegrationTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}

	ASMSourceLoader loader;
	ASMTokeniser tokeniser;
	ASMParser parser;
};

class ASMParserIntegrationExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    ASMSourceLoader loader;
    ASMTokeniser tokeniser;
    ASMParser parser;
};

template<typename T>
T* get_operand( const ASTExpression& expr )
{
    auto ptr = std::get_if<T>( &expr.expression );
    ASSERT_NE( ptr, nullptr );
    return ptr;
}

TEST_F(ASMParserIntegrationTest, FourLiner)
{
	std::string input =
R"(start:
    LD V0, 10
    ADD V0, 5
    JP start
)";

	std::stringstream is( input );

	ASMSource source = loader.load( is );
	ASMTokens tokens = tokeniser.tokenise( source );
	ASTProgram program = parser.parse_asm_tokens( tokens );

	ASSERT_EQ( program.size(), 4);

	{
		EXPECT_TRUE( program[0].label.has_value() );
		EXPECT_EQ( program[0].label.value().name, "start" );
		EXPECT_EQ( program[0].line, 1 );
	}
	{
		EXPECT_FALSE( program[1].label.has_value() );
		EXPECT_EQ( program[1].line, 2 );

		auto instr_ptr = std::get_if<ASTInstruction>(&program[1].body);
		ASSERT_NE(instr_ptr, nullptr);

		EXPECT_EQ( instr_ptr->mnemonic, "LD" );

		ASSERT_EQ( instr_ptr->operands.size(), 2 );
		{
			auto operand_ptr = std::get_if<IdentifierExpr>(&instr_ptr->operands[0].expression);
			ASSERT_NE(operand_ptr, nullptr);
			EXPECT_EQ( operand_ptr->text, "V0" );
		}
		{
			auto operand_ptr = std::get_if<NumberExpr>(&instr_ptr->operands[1].expression);
			ASSERT_NE(operand_ptr, nullptr);
			EXPECT_EQ( operand_ptr->value, 10 );
		}
	}
	{
		EXPECT_FALSE( program[2].label.has_value() );
		EXPECT_EQ( program[2].line, 3 );

		auto instr_ptr = std::get_if<ASTInstruction>(&program[2].body);
		ASSERT_NE(instr_ptr, nullptr);

		EXPECT_EQ( instr_ptr->mnemonic, "ADD" );

		ASSERT_EQ( instr_ptr->operands.size(), 2 );
		{
			auto operand_ptr = std::get_if<IdentifierExpr>(&instr_ptr->operands[0].expression);
			ASSERT_NE(operand_ptr, nullptr);
			EXPECT_EQ( operand_ptr->text, "V0" );
		}
		{
			auto operand_ptr = std::get_if<NumberExpr>(&instr_ptr->operands[1].expression);
			ASSERT_NE(operand_ptr, nullptr);
			EXPECT_EQ( operand_ptr->value, 5 );
		}
	}
	{
		EXPECT_FALSE( program[3].label.has_value() );
		EXPECT_EQ( program[3].line, 4 );

		auto instr_ptr = std::get_if<ASTInstruction>(&program[3].body);
		ASSERT_NE(instr_ptr, nullptr);

		EXPECT_EQ( instr_ptr->mnemonic, "JP" );

		ASSERT_EQ( instr_ptr->operands.size(), 1 );
		{
			auto operand_ptr = std::get_if<IdentifierExpr>(&instr_ptr->operands[0].expression);
			ASSERT_NE(operand_ptr, nullptr);
			EXPECT_EQ( operand_ptr->text, "start" );
		}
	}
}

// Helper for operand assertions
template<typename T>
T* get_operand( ASTExpression& expr )
{
    T* ptr = std::get_if<T>(&expr.expression);
    if (!ptr) {
        throw std::runtime_error("Operand type mismatch in test helper");
    }
    return ptr;
}

TEST_F(ASMParserIntegrationExtendedTest, DirectiveWithOperands)
{
    std::string input =
R"(.DW 0x10, 0x20, 0x30
)";

    std::stringstream is(input);

    ASMSource source = loader.load(is);
    ASMTokens tokens = tokeniser.tokenise(source);
    ASTProgram program = parser.parse_asm_tokens(tokens);

    ASSERT_EQ(program.size(), 1);

    auto dir_ptr = std::get_if<ASTDirective>(&program[0].body);
    ASSERT_NE(dir_ptr, nullptr);

    EXPECT_EQ(dir_ptr->name, ".DW");
    ASSERT_EQ(dir_ptr->args.size(), 3);

    EXPECT_EQ(get_operand<NumberExpr>(dir_ptr->args[0])->value, 0x10);
    EXPECT_EQ(get_operand<NumberExpr>(dir_ptr->args[1])->value, 0x20);
    EXPECT_EQ(get_operand<NumberExpr>(dir_ptr->args[2])->value, 0x30);
}

TEST_F(ASMParserIntegrationExtendedTest, VariableDefinitionEqu)
{
    std::string input =
R"(X_POS EQU 0x10
Y_POS EQU 0x20
)";

    std::stringstream is(input);

    ASMSource source = loader.load(is);
    ASMTokens tokens = tokeniser.tokenise(source);
    ASTProgram program = parser.parse_asm_tokens(tokens);

    ASSERT_EQ(program.size(), 2);

    auto equ1 = std::get_if<ASTEqu>(&program[0].body);
    ASSERT_NE(equ1, nullptr);
    EXPECT_EQ(equ1->name, "X_POS");
    EXPECT_EQ(get_operand<NumberExpr>(equ1->value)->value, 0x10);

    auto equ2 = std::get_if<ASTEqu>(&program[1].body);
    ASSERT_NE(equ2, nullptr);
    EXPECT_EQ(equ2->name, "Y_POS");
    EXPECT_EQ(get_operand<NumberExpr>(equ2->value)->value, 0x20);
}

TEST_F(ASMParserIntegrationExtendedTest, CommentsAndWhitespace)
{
    std::string input =
R"(start:          ; label comment
    LD V0, 0x10   ; instruction comment
    ; full line comment
    JP start
)";

    std::stringstream is(input);

    ASMSource source = loader.load(is);
    ASMTokens tokens = tokeniser.tokenise(source);
    ASTProgram program = parser.parse_asm_tokens(tokens);

    ASSERT_EQ(program.size(), 3);

    // Label
    EXPECT_TRUE(program[0].label.has_value());
    EXPECT_EQ(program[0].label.value().name, "start");

    // LD instruction
    auto ld_ptr = std::get_if<ASTInstruction>(&program[1].body);
    ASSERT_NE(ld_ptr, nullptr);
    EXPECT_EQ(ld_ptr->mnemonic, "LD");
    ASSERT_EQ(ld_ptr->operands.size(), 2);
    EXPECT_EQ(get_operand<IdentifierExpr>(ld_ptr->operands[0])->text, "V0");
    EXPECT_EQ(get_operand<NumberExpr>(ld_ptr->operands[1])->value, 0x10);

    // Full line comment should be skipped by tokeniser

    // JP instruction
    auto jp_ptr = std::get_if<ASTInstruction>(&program[2].body);
    ASSERT_NE(jp_ptr, nullptr);
    EXPECT_EQ(jp_ptr->mnemonic, "JP");
    ASSERT_EQ(jp_ptr->operands.size(), 1);
    EXPECT_EQ(get_operand<IdentifierExpr>(jp_ptr->operands[0])->text, "start");
}

TEST_F(ASMParserIntegrationExtendedTest, SimpleExpressionOperands)
{
    std::string input =
R"(LD V0, 10+5*2
)";

    std::stringstream is(input);

    ASMSource source = loader.load(is);
    ASMTokens tokens = tokeniser.tokenise(source);
    ASTProgram program = parser.parse_asm_tokens(tokens);

    ASSERT_EQ(program.size(), 1);

    auto instr_ptr = std::get_if<ASTInstruction>(&program[0].body);
    ASSERT_NE(instr_ptr, nullptr);
    ASSERT_EQ(instr_ptr->operands.size(), 2);

    EXPECT_EQ(get_operand<IdentifierExpr>(instr_ptr->operands[0])->text, "V0");

    auto expr_ptr = std::get_if<ASTBinaryExpr>(&instr_ptr->operands[1].expression);
    ASSERT_NE(expr_ptr, nullptr);

    // Check root operator
    EXPECT_EQ(expr_ptr->op, '+');

    // Left side = 10
    auto lhs_num = std::get_if<NumberExpr>(&expr_ptr->lhs->expression);
    ASSERT_NE(lhs_num, nullptr);
    EXPECT_EQ(lhs_num->value, 10);

    // Right side = 5*2
    auto rhs_bin = std::get_if<ASTBinaryExpr>(&expr_ptr->rhs->expression);
    ASSERT_NE(rhs_bin, nullptr);
    EXPECT_EQ(rhs_bin->op, '*');
    EXPECT_EQ(std::get<NumberExpr>(rhs_bin->lhs->expression).value, 5);
    EXPECT_EQ(std::get<NumberExpr>(rhs_bin->rhs->expression).value, 2);
}
