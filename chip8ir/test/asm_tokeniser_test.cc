/*
 * asm_tokeniser_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/tokeniser.h"
#include "ir/chip8formats.h"

class ASMTokeniserTestTest : public ::testing::Test {
protected:
	void SetUp() override {

	}
	void TearDown() override {
	}

	ASMTokeniser tokeniser;
	ASMSource source;
};

TEST_F(ASMTokeniserTestTest, EmptyInput)
{
	ASMTokens tokens = tokeniser.tokenise( ASMSource {} );

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, ASMToken::Type::END_OF_INPUT);
}


TEST_F(ASMTokeniserTestTest, LabelOnly)
{
	source.push_back( ASMLine { "start:", 1 } );
	ASMTokens tokens = tokeniser.tokenise( source );

    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].type, ASMToken::Type::LABEL);
    EXPECT_EQ(tokens[0].lexeme, "start");

    EXPECT_EQ(tokens.back().type, ASMToken::Type::END_OF_INPUT);
}

TEST_F(ASMTokeniserTestTest, InstructionWithArguments)
{
	source.push_back( ASMLine { "LD V0, 10", 1 } );
	ASMTokens tokens = tokeniser.tokenise( source );

    ASSERT_GE(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, ASMToken::Type::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "LD");

    EXPECT_EQ(tokens[1].type, ASMToken::Type::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "V0");

    EXPECT_EQ(tokens[2].type, ASMToken::Type::COMMA);
    EXPECT_EQ(tokens[2].lexeme, ",");

	EXPECT_EQ(tokens[3].type, ASMToken::Type::NUMBER);
    EXPECT_EQ(tokens[3].lexeme, "10");
}

TEST_F(ASMTokeniserTestTest, Directive)
{
	source.push_back( ASMLine { ".DB 0xFF, 0x01", 1 } );
	ASMTokens tokens = tokeniser.tokenise( source );

    EXPECT_EQ(tokens[0].type, ASMToken::Type::DIRECTIVE);
    EXPECT_EQ(tokens[0].lexeme, ".DB");

    EXPECT_EQ(tokens[1].lexeme, "0xFF");
    EXPECT_EQ(tokens[2].lexeme, ",");
    EXPECT_EQ(tokens[3].lexeme, "0x01");
}

TEST_F(ASMTokeniserTestTest, CommentIsTokenised)
{
	source.push_back( ASMLine { "LD V0, 1 ; comment here", 1 } );
	ASMTokens tokens = tokeniser.tokenise( source );

    bool found_comment = false;
    for (const auto& tok : tokens)
        if (tok.type == ASMToken::Type::COMMENT)
            found_comment = true;

    EXPECT_TRUE(found_comment);
}

TEST_F(ASMTokeniserTestTest, Operators)
{
	source.push_back( ASMLine { "+-*/", 1 } );
	ASMTokens tokens = tokeniser.tokenise( source );

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, ASMToken::Type::OPERATOR); EXPECT_EQ(tokens[0].lexeme, "+");
    EXPECT_EQ(tokens[1].type, ASMToken::Type::OPERATOR); EXPECT_EQ(tokens[1].lexeme, "-");
    EXPECT_EQ(tokens[2].type, ASMToken::Type::OPERATOR); EXPECT_EQ(tokens[2].lexeme, "*");
    EXPECT_EQ(tokens[3].type, ASMToken::Type::OPERATOR); EXPECT_EQ(tokens[3].lexeme, "/");

    EXPECT_EQ(tokens[4].type, ASMToken::Type::END_OF_INPUT);
}
