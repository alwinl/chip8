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

class SymbolTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        
    }
    void TearDown() override {
    }
};

// TEST_F(SymbolTableTest, EmptyLine)
// {
//     SymbolTable symbol_table;

//     std::string line = "";
//     std::stringstream source(line);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(0, symbol_table.size());
    
// }

// TEST_F(SymbolTableTest, NoLabel)
// {
//     SymbolTable symbol_table;

//     std::string line = "\tJP main";
//     std::stringstream source(line);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(0, symbol_table.size());
// }

// TEST_F(SymbolTableTest, ColonInComment)
// {
//     SymbolTable symbol_table;

//     std::string line = "\tJP main\t; hello:";
//     std::stringstream source(line);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(0, symbol_table.size());
// }


// TEST_F(SymbolTableTest, SimpleLabel)
// {
//     SymbolTable symbol_table;

//     std::string line = "_start: JP main";
//     std::stringstream source(line);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(1, symbol_table.size());

//     EXPECT_EQ(0x0200, symbol_table.get_address("_start"));
// }

// TEST_F(SymbolTableTest, TwoLabels)
// {
//     SymbolTable symbol_table;

//     std::string line = R"(
//         _start: JP main
//         main: JP _start
//     )";
//     std::stringstream source(line);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(2, symbol_table.size());

//     EXPECT_EQ(0x0200, symbol_table.get_address("_start"));
//     EXPECT_EQ(0x0202, symbol_table.get_address("main"));
// }

// TEST_F(SymbolTableTest, TwoLabelsWithInstructionGap)
// {
//     SymbolTable symbol_table;

//     std::string input = R"(
//         _start: JP main
//                 MOV V0,#15
//         main:   JP _start
//     )";    

//     std::stringstream source(input);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(2, symbol_table.size());

//     EXPECT_EQ(0x0200, symbol_table.get_address("_start"));
//     EXPECT_EQ(0x0204, symbol_table.get_address("main"));
// }

// TEST_F(SymbolTableTest, TwoLabelsWithBlankLine)
// {
//     SymbolTable symbol_table;

//     std::string input = R"(
//         _start: JP main
 
//         main:   JP _start
//     )";    

//     std::stringstream source(input);

//     symbol_table.fill_table(source);

//     EXPECT_EQ(2, symbol_table.size());

//     EXPECT_EQ(0x0200, symbol_table.get_address("_start"));
//     EXPECT_EQ(0x0202, symbol_table.get_address("main"));
// }