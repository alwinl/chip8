/*
 * symboltabletest.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler.h"

TEST(SymbolTable, NoEntries)
{
    SymbolTable symbol_table;

    EXPECT_EQ(0, symbol_table.size());
}

TEST(SymbolTable, AddSingleLabel)
{
    SymbolTable symbol_table;

    symbol_table.add_label("main", 0x0200);

    EXPECT_EQ(1, symbol_table.size());
    EXPECT_EQ(0x0200, symbol_table.get_address("main"));
}

TEST(SymbolTable, AddMultipleLabels)
{
    SymbolTable symbol_table;

    symbol_table.add_label("main", 0x0200);
    symbol_table.add_label("_start", 0x0202);

    EXPECT_EQ(2, symbol_table.size());
    EXPECT_EQ(0x0200, symbol_table.get_address("main"));
    EXPECT_EQ(0x0202, symbol_table.get_address("_start"));
}

TEST(SymbolTable, AddDuplicateLabel)
{
    SymbolTable symbol_table;

    symbol_table.add_label("main", 0x0200);
    EXPECT_THROW(symbol_table.add_label("main", 0x0202), std::invalid_argument);
}

TEST(SymbolTable, GetAddressNonExistentLabel)
{
    SymbolTable symbol_table;

    symbol_table.add_label("main", 0x0200);

    EXPECT_THROW(symbol_table.get_address("non_existent"), std::runtime_error);
}
