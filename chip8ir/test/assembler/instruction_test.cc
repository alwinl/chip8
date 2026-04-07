/*
 * instruction_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/chip8ir.h"

class InstructionTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}
};

TEST_F(InstructionTest, LD_Immediate)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "LD";
    inst.operands.emplace_back( ASTExpression{ IdentifierExpr{"V1"}, 0 } );
    inst.operands.emplace_back( ASTExpression{ NumberExpr{42}, 0 } );

    auto ir = get_dispatcher().at("LD")(inst, symbols);

    EXPECT_EQ(ir.opcode(), Opcode::LD_Imm);
}

TEST_F(InstructionTest, ADD_Register)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "ADD";
    inst.operands.emplace_back( ASTExpression{ IdentifierExpr{"V1"}, 0 } );
    inst.operands.emplace_back( ASTExpression{ IdentifierExpr{"V2"}, 0 } );

    auto ir = get_dispatcher().at("ADD")(inst, symbols);

    EXPECT_EQ(ir.opcode(), Opcode::ADD_Reg);
}

TEST_F(InstructionTest, Jump)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "JP";
    inst.operands.emplace_back( ASTExpression{ NumberExpr{0x300}, 0 } );

    auto ir = get_dispatcher().at("JP")(inst, symbols);

    EXPECT_EQ(ir.opcode(), Opcode::JP);
}
