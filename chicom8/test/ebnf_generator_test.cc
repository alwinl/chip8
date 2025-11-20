/*
 * ebnf_generator_test.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

 // TEST(EbnfCompilerTest, GenerateAstClassOutput) {
//     RuleNode rule;
//     rule.name = "<expr>";
//     rule.productions = { {"\"42\""}, {"<ident>"} };

//     std::string output = capture_stdout([&]() {
//         generateAstClass(std::cout, rule);
//     });

//     // Check that struct and variant are present
//     EXPECT_NE(output.find("struct <Expr>"), std::string::npos);
//     EXPECT_NE(output.find("std::variant<"), std::string::npos);
//     EXPECT_NE(output.find("struct \"42\""), std::string::npos);
//     EXPECT_NE(output.find("struct <Ident>"), std::string::npos);
// }

