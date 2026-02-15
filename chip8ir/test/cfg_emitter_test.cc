/*
 * cfg_emitter_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/chip8ir.h"
#include "ir/cfg_emitter.h"

#include <fstream>

class CFGEmitterTest : public ::testing::Test
{
protected:
	IRProgram ir;
	CFGEmitter emitter;

	void SetUp() override
	{
		ir.origin = 0x200;

		// Add some instructions
		ir.elements.push_back( InstructionElement{0x200, Instruction::make_ld( Reg{0}, Imm{5} )} );
		ir.elements.push_back( InstructionElement{0x202, Instruction::make_skip_eq( Reg{0}, Imm{5} )} );
		ir.elements.push_back( InstructionElement{0x204, Instruction::make_jump( Addr{ 0x208} )} );
		ir.elements.push_back( InstructionElement{0x208, Instruction::make_ld( Reg{1}, Imm{1} )} );
	}

	void TearDown() override {}
};

TEST_F(CFGEmitterTest, create_plantuml )
{
	std::ofstream plantuml_file("cfg_plantuml.plantuml");

    emitter.emit(plantuml_file, ir, CFGEmitter::OutputMode::PlantUML);

    plantuml_file.close();
}

TEST_F(CFGEmitterTest, create_dot )
{
	std::ofstream dot_file("cfg_dot.dot");

    emitter.emit(dot_file, ir, CFGEmitter::OutputMode::Dot);

    dot_file.close();
}