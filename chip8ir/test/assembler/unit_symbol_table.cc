/*
 * unit_symbol_table.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/symbol_table.h"

TEST(UnitSymbolTable, DefineLabel)
{
	ASMSymbolTable symbols;

	symbols.define_label("start", 0x200);

	EXPECT_EQ(symbols.get_value("start"), 0x200);
	EXPECT_EQ(symbols.get_label(0x200), "start");
}

TEST(UnitSymbolTable, DefineConstant)
{
	ASMSymbolTable symbols;

	symbols.define_constant("SCREEN_WIDTH", 64);

	EXPECT_EQ(symbols.get_value("SCREEN_WIDTH"), 64);
}

TEST(UnitSymbolTable, DefineMultipleLabels)
{
	ASMSymbolTable symbols;

	symbols.define_label("start", 0x200);
	symbols.define_label("loop", 0x208);

	EXPECT_EQ(symbols.get_value("start"), 0x200);
	EXPECT_EQ(symbols.get_value("loop"), 0x208);
	EXPECT_EQ(symbols.get_label(0x200), "start");
	EXPECT_EQ(symbols.get_label(0x208), "loop");
}

TEST(UnitSymbolTable, DefineMultipleConstants)
{
	ASMSymbolTable symbols;

	symbols.define_constant("X_POS", 10);
	symbols.define_constant("Y_POS", 20);

	EXPECT_EQ(symbols.get_value("X_POS"), 10);
	EXPECT_EQ(symbols.get_value("Y_POS"), 20);
}

TEST(UnitSymbolTable, LabelAndConstantCanCoexist)
{
	ASMSymbolTable symbols;

	symbols.define_label("start", 0x200);
	symbols.define_constant("SCREEN_WIDTH", 64);

	EXPECT_EQ(symbols.get_value("start"), 0x200);
	EXPECT_EQ(symbols.get_label(0x200), "start");
	EXPECT_EQ(symbols.get_value("SCREEN_WIDTH"), 64);
}

TEST(UnitSymbolTable, MissingLabelReturnsEmptyString)
{
	ASMSymbolTable symbols;

	EXPECT_EQ(symbols.get_label(0x200), "");
}

TEST(UnitSymbolTable, MissingSymbolReturnsSentinel)
{
	ASMSymbolTable symbols;

	EXPECT_EQ(symbols.get_value("missing"), UINT16_MAX);
}

TEST(UnitSymbolTable, DuplicateLabelDoesNotOverwriteExistingDefinition)
{
	ASMSymbolTable symbols;

	symbols.define_label("start", 0x200);
	symbols.define_label("start", 0x300);

	EXPECT_EQ(symbols.get_value("start"), 0x200);
	EXPECT_EQ(symbols.get_label(0x200), "start");
	EXPECT_EQ(symbols.get_label(0x300), "");
}

TEST(UnitSymbolTable, DuplicateConstantDoesNotOverwriteExistingDefinition)
{
	ASMSymbolTable symbols;

	symbols.define_constant("SCREEN_WIDTH", 64);
	symbols.define_constant("SCREEN_WIDTH", 128);

	EXPECT_EQ(symbols.get_value("SCREEN_WIDTH"), 64);
}
