/*
 * parser_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "parser.h"

#include <gtest/gtest.h>

TEST(ParserTest, ParsesLetStatement) {
    // Tokeniser tokeniser("let x = 42;");
    // Parser parser(tokeniser);

    // std::unique_ptr<Stmt> stmt = parser.parse_statement();

    // ASSERT_NE(stmt, nullptr);
    // ASSERT_EQ(stmt->type, StmtType::Let);

    // LetStmt* let_stmt = dynamic_cast<LetStmt*>(stmt.get());
    // ASSERT_EQ(let_stmt->name, "x");

    // NumberLiteral* value = dynamic_cast<NumberLiteral*>(let_stmt->initializer.get());
    // ASSERT_EQ(value->value, 42);
}