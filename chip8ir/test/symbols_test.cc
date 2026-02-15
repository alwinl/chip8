/*
 * targets_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include "ir/symbol_table.h"

class SymbolsTest : public ::testing::Test {
protected:
    SymbolTable symbols;
};

TEST_F(SymbolsTest, add_empty_optional_does_nothing)
{
    symbols.add(std::nullopt);
    symbols.sort_vectors();

    EXPECT_EQ(symbols.get_label(0x200), "");
}

TEST_F(SymbolsTest, add_single_target)
{
    symbols.add(DecodedSymbol{0x234, JUMP});
    symbols.sort_vectors();

    EXPECT_EQ(symbols.get_label(0x234), "LABEL0");
}

TEST_F(SymbolsTest, multiple_targets_same_kind)
{
    symbols.add(DecodedSymbol{0x300, SUBROUTINE});
    symbols.add(DecodedSymbol{0x200, SUBROUTINE});
    symbols.add(DecodedSymbol{0x280, SUBROUTINE});

    symbols.sort_vectors();

    EXPECT_EQ(symbols.get_label(0x200), "FUNC0");
    EXPECT_EQ(symbols.get_label(0x280), "FUNC1");
    EXPECT_EQ(symbols.get_label(0x300), "FUNC2");
}

TEST_F(SymbolsTest, duplicate_targets_are_removed)
{
    symbols.add(DecodedSymbol{0x234, JUMP});
    symbols.add(DecodedSymbol{0x234, JUMP});
    symbols.add(DecodedSymbol{0x234, JUMP});

    symbols.sort_vectors();

    EXPECT_EQ(symbols.get_label(0x234), "LABEL0");
}

TEST_F(SymbolsTest, same_address_different_kinds)
{
    symbols.add(DecodedSymbol{0x400, JUMP});
    symbols.add(DecodedSymbol{0x400, SUBROUTINE});

    symbols.sort_vectors();

    EXPECT_EQ(symbols.get_label(0x400), "FUNC0"); // SUBROUTINE checked first?
}

TEST_F(SymbolsTest, unknown_address_returns_empty)
{
    symbols.add(DecodedSymbol{0x200, JUMP});
    symbols.sort_vectors();

    EXPECT_EQ(symbols.get_label(0x201), "");
}

TEST_F(SymbolsTest, indexed_list_contents)
{
    symbols.add(DecodedSymbol{0x300, INDEXED});
    symbols.add(DecodedSymbol{0x200, INDEXED});
    symbols.add(DecodedSymbol{0x200, INDEXED});

    symbols.sort_vectors();

    auto list = symbols.get_index_list();
    ASSERT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 0x200);
    EXPECT_EQ(list[1], 0x300);
}

#ifndef NDEBUG
TEST_F(SymbolsTest, get_label_without_sort_asserts)
{
    symbols.add(DecodedSymbol{0x200, JUMP});
    EXPECT_DEATH(symbols.get_label(0x200), ".*");
}
#endif
