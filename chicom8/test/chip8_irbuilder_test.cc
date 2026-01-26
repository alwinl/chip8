/*
 * chip8_irbuilder_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include "ir_builder.h"

#include <gtest/gtest.h>

class IRBuilderLabelTest : public ::testing::Test {
protected:
	IRBuilder ir;

	void SetUp() override {

	}
	void TearDown() override {
	}
};

TEST_F(IRBuilderLabelTest, FirstLabelIsZero) {
	EXPECT_EQ(ir.new_label("while"), "while0");
}

TEST_F(IRBuilderLabelTest, SequentialLabelsIncrement)
{
    EXPECT_EQ(ir.new_label("while"), "while0");
    EXPECT_EQ(ir.new_label("while"), "while1");
    EXPECT_EQ(ir.new_label("while"), "while2");
}

TEST_F(IRBuilderLabelTest, DifferentStemsAreIndependent)
{
    EXPECT_EQ(ir.new_label("while"), "while0");
    EXPECT_EQ(ir.new_label("if"),    "if0");
    EXPECT_EQ(ir.new_label("while"), "while1");
    EXPECT_EQ(ir.new_label("if"),    "if1");
}

TEST_F(IRBuilderLabelTest, InterleavedLabelGeneration)
{
    EXPECT_EQ(ir.new_label("while"), "while0");
    EXPECT_EQ(ir.new_label("if"),    "if0");
    EXPECT_EQ(ir.new_label("while"), "while1");
    EXPECT_EQ(ir.new_label("endif"), "endif0");
    EXPECT_EQ(ir.new_label("if"),    "if1");
}
TEST_F(IRBuilderLabelTest, LabelsDoNotReset)
{
    ir.new_label("loop");
    ir.new_label("loop");

    EXPECT_EQ(ir.new_label("loop"), "loop2");
}
