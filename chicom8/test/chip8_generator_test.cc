/*
 * chip8_generator_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include "generator.h"

// class Chip8GeneratorTest : public ::testing::Test {
// protected:
// 	void SetUp() override {

// 	}
// 	void TearDown() override {
// 	}
// };

// TEST_F(Chip8GeneratorTest, Simple_AST)
// {
// 	std::string source(
// R"(
// 		func main() {
// 		}
// )"
// 	);

// 	std::string expected(
// R"(
// start:

// end:	JP end
// )"
// 	);

// 	Generator generator;

// 	std::string actual = generator.emit_assembly( source );

// 	EXPECT_EQ( expected, actual );
// }

// TEST_F(Chip8GeneratorTest, While_AST)
// {
// 	std::string source(
// R"(
// 		func main() {
// 			return;
// 		}
// )"
// 	);

// 	std::string expected(
// R"(
// start:

// end:	JP end
// )"
// 	);

// 	Generator generator;

// 	std::string actual = generator.emit_assembly( source );

// 	EXPECT_EQ( expected, actual );
// }