/*
 * chip8_irbuilder2_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include "ir_builder.h"

class IRBuilderTest : public ::testing::Test {
protected:
	IRBuilder ir;

    std::string dump() {
        std::ostringstream oss;
        oss << ir;
        return oss.str();
    }
};

TEST_F(IRBuilderTest, EmitLabel)
{
    ir.emit_label("start");

    EXPECT_EQ(dump(),
        "start:\n"
    );
}

TEST_F(IRBuilderTest, EmitJump)
{
    ir.emit_jump("end");

    EXPECT_EQ(dump(),
        "JP end\n"
    );
}

TEST_F(IRBuilderTest, MinimalProgramShape)
{
    ir.emit_label("start");
    ir.emit_label("end");
    ir.emit_jump("end");

    EXPECT_EQ(dump(),
        "start:\n"
        "end:\n"
        "JP end\n"
    );
}

TEST_F(IRBuilderTest, IfStatementIRShape)
{
    Expr* cond = nullptr;

    auto else_l = ir.new_label("else");
    auto end_l  = ir.new_label("endif");

    ir.emit_jump_if_false(cond, else_l);
    ir.emit_jump(end_l);
    ir.emit_label(else_l);
    ir.emit_label(end_l);

    auto out = dump();

    EXPECT_NE(out.find("else0"), std::string::npos);
    EXPECT_NE(out.find("endif0"), std::string::npos);

    // Order matters
    EXPECT_LT(out.find("else0"), out.find("endif0"));
}

TEST_F(IRBuilderTest, WhileLoopIRShape)
{
    Expr* cond = nullptr;

    auto loop = ir.new_label("while");
    auto exit = ir.new_label("endwhile");

    ir.emit_label(loop);
    ir.emit_jump_if_false(cond, exit);
    ir.emit_jump(loop);
    ir.emit_label(exit);

    auto out = dump();

    EXPECT_NE(out.find("while0:"), std::string::npos);
    EXPECT_NE(out.find("endwhile0:"), std::string::npos);

    // loop label must come before exit label
    EXPECT_LT(out.find("while0:"), out.find("endwhile0:"));
}

TEST_F(IRBuilderTest, NestedWhileIfProducesUniqueLabels)
{
    Expr* a = nullptr;
    Expr* b = nullptr;

    auto while_l = ir.new_label("while");
    auto endwhile_l = ir.new_label("endwhile");

    ir.emit_label(while_l);

    auto else_l = ir.new_label("else");
    auto endif_l = ir.new_label("endif");

    ir.emit_jump_if_false(a, endwhile_l);
    ir.emit_jump_if_false(b, else_l);
    ir.emit_jump(endif_l);
    ir.emit_label(else_l);
    ir.emit_label(endif_l);

    ir.emit_jump(while_l);
    ir.emit_label(endwhile_l);

    auto out = dump();

    EXPECT_NE(out.find("while0"), std::string::npos);
    EXPECT_NE(out.find("else0"), std::string::npos);
    EXPECT_NE(out.find("endif0"), std::string::npos);
    EXPECT_NE(out.find("endwhile0"), std::string::npos);
}

TEST_F(IRBuilderTest, DeterministicLabelGeneration)
{
    EXPECT_EQ(ir.new_label("x"), "x0");
    EXPECT_EQ(ir.new_label("x"), "x1");
    EXPECT_EQ(ir.new_label("y"), "y0");
    EXPECT_EQ(ir.new_label("x"), "x2");
}

TEST_F(IRBuilderTest, GoldenIRDump)
{
    Expr* cond = nullptr;

    ir.emit_label("start");

    auto loop = ir.new_label("while");
    auto exit = ir.new_label("endwhile");

    ir.emit_label(loop);
    ir.emit_jump_if_false(cond, exit);
    ir.emit_jump(loop);
    ir.emit_label(exit);

    ir.emit_label("end");
    ir.emit_jump("end");

    EXPECT_EQ(dump(),
        "start:\n"
        "while0:\n"
        "JumpIfFalse endwhile0\n"
        "JP while0\n"
        "endwhile0:\n"
        "end:\n"
        "JP end\n"
    );
}
