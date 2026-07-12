/*
 * asm_integration_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/chip8formats.h"
#include "assembler/assembler.h"
#include "assembler/loader.h"

class AssemblerIntegrationTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}

	IRBundle bundle_builder(const std::string& text)
	{
	    std::istringstream is(text);
		ASMSource source = load_assembly_source(is);
		return Assembler().build_ir(source);
	}
};

TEST_F(AssemblerIntegrationTest, SimpleProgram)
{
	auto [ir, _] = bundle_builder(
R"(
	.ORG 0x200
	CLS
	JP 0x208
	.DB 1,2,3,4
	LD V2, 0xAA
)"
	);

    ASSERT_EQ(ir.elements.size(), 4);

    EXPECT_EQ(std::get<InstructionElement>(ir.elements[0]).address, 0x200);
    EXPECT_EQ(std::get<InstructionElement>(ir.elements[1]).address, 0x202);
    EXPECT_EQ(std::get<DataElement>(ir.elements[2]).address, 0x204);
    EXPECT_EQ(std::get<InstructionElement>(ir.elements[3]).address, 0x208);
}

TEST_F(AssemblerIntegrationTest, LabelResolution)
{
	auto [ir, _] = bundle_builder(
R"(
start:
	JP target
	CLS
target:
	RET
)"
	);

    const auto& jp = std::get<InstructionElement>(ir.elements[0]);

    auto operands = jp.instruction.operands();

    ASSERT_EQ(operands.size(), 1);
    EXPECT_EQ(std::get<Addr>(operands[0]).value, 0x204);
}

TEST_F(AssemblerIntegrationTest, ExpressionEvaluation)
{
	auto [ir, _] = bundle_builder(
R"(
	value EQU 10
	LD V1, value + 5
)"
	);

	const auto& inst = std::get<InstructionElement>(ir.elements[0]);

    auto operands = inst.instruction.operands();

    EXPECT_EQ(std::get<Imm>(operands[1]).value, 15);
}

TEST_F(AssemblerIntegrationTest, SE_RegisterVsImmediate)
{
	auto [ir, _] = bundle_builder(
R"(
	SE V1, V2
	SE V1, 10
)"
	);

	EXPECT_EQ(std::get<InstructionElement>(ir.elements[0]).instruction.opcode(), Opcode::SE_Reg);
    EXPECT_EQ(std::get<InstructionElement>(ir.elements[1]).instruction.opcode(), Opcode::SE_Imm);
}

TEST_F(AssemblerIntegrationTest, JumpIndexedVsDirect)
{
	auto [ir, _] = bundle_builder(
R"(
	JP V0, 0x300
	JP 0x400
)"
	);

	EXPECT_EQ(std::get<InstructionElement>(ir.elements[0]).instruction.opcode(), Opcode::JP_V0);
    EXPECT_EQ(std::get<InstructionElement>(ir.elements[1]).instruction.opcode(), Opcode::JP);
}

TEST_F(AssemblerIntegrationTest, OrgDirective)
{
	auto [ir, _] = bundle_builder(
R"(
	.ORG 0x300
	CLS
)"
	);

    EXPECT_EQ(std::get<InstructionElement>(ir.elements[0]).address, 0x300);
}

TEST_F(AssemblerIntegrationTest, DataDirective)
{
	auto [ir, _] = bundle_builder(
R"(
	.DB 1,2,3,4
)"
	);

    const auto& data = std::get<DataElement>(ir.elements[0]);

    ASSERT_EQ(data.byte_run.size(), 4);
    EXPECT_EQ(data.byte_run[0], 1);
    EXPECT_EQ(data.byte_run[3], 4);
}

TEST_F(AssemblerIntegrationTest, CommentsAndWhitespace)
{
	auto [ir, _] = bundle_builder(
R"(
	; full line comment
	CLS ; inline comment
	; another comment
)"
	);

    ASSERT_EQ(ir.elements.size(), 1);
}