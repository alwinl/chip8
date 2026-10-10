/*
 * unit_instruction_generator.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include "assembler/instruction_generator.h"

#include "ir/chip8ir.h"

TEST(UnitInstruction, LD_Immediate)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "LD";
    inst.operands.emplace_back( ASTExpression{ ASTIdentifierExpr{"V1"}, 0 } );
    inst.operands.emplace_back( ASTExpression{ ASTNumberExpr{42}, 0 } );

	EXPECT_NO_THROW({
		Instruction ir = generate_instruction(inst, symbols);
		EXPECT_EQ(ir.opcode(), Opcode::LD_Imm);
	});
}

TEST(UnitInstruction, ADD_Register)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "ADD";
    inst.operands.emplace_back( ASTExpression{ ASTIdentifierExpr{"V1"}, 0 } );
    inst.operands.emplace_back( ASTExpression{ ASTIdentifierExpr{"V2"}, 0 } );

	EXPECT_NO_THROW({
		Instruction ir = generate_instruction(inst, symbols);
		EXPECT_EQ(ir.opcode(), Opcode::ADD_Reg);
	});
}

TEST(UnitInstruction, Jump)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "JP";
    inst.operands.emplace_back( ASTExpression{ ASTNumberExpr{0x300}, 0 } );

	EXPECT_NO_THROW({
		Instruction ir = generate_instruction(inst, symbols);
		EXPECT_EQ(ir.opcode(), Opcode::JP);
	});
}

TEST(UnitInstruction, UnknownMnemonic)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "UNKNOWN";

    EXPECT_THROW(
        generate_instruction(inst, symbols),
        std::runtime_error
    );
}

TEST(UnitInstruction, InvalidRegister)
{
    ASMSymbolTable symbols;

    ASTInstruction inst;
    inst.mnemonic = "OR";
    inst.operands.emplace_back( ASTExpression{ ASTIdentifierExpr{"V99"}, 0 } );
    inst.operands.emplace_back( ASTExpression{ ASTIdentifierExpr{"V99"}, 0 } );

    EXPECT_THROW(
        generate_instruction(inst, symbols),
        std::runtime_error
    );
}

