/*
 * unit_symbol_collector.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/symbol_collector.h"

namespace {

TEST(UnitSymbolCollector, EmptyProgram)
{
    ASTProgram program;

    auto symbols = collect_symbols(program);

    // No labels or constants to resolve.
    EXPECT_EQ(symbols.get_label(0x200), "");
}

TEST(UnitSymbolCollector, InstructionAdvancesAddressByTwo)
{
    ASTProgram program;

    // First instruction at 0x200; label on the next element at 0x202.
    program.emplace_back( ASTElement{ 1, std::nullopt, ASTInstruction{"NOP", 5, {}} } );
    program.emplace_back( ASTElement{ 2, ASTLabel{"next", 0}, ASTEmpty{} } );

    auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("next"), 0x202);
}

TEST(UnitSymbolCollector, ByteDirectiveAdvancesAddressByByteCount)
{
	ASTDirective directive { ".DB", 5,};

	directive.args.emplace_back( ASTNumberExpr{1}, 0 );
	directive.args.emplace_back( ASTNumberExpr{2}, 0 );
	directive.args.emplace_back( ASTNumberExpr{3}, 0 );

    ASTProgram program;

	program.emplace_back( ASTElement{ 1, std::nullopt, std::move(directive) } );
    program.emplace_back( ASTElement{ 2, ASTLabel{"after_data", 0}, ASTEmpty{} } );

    auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("after_data"), 0x203);
}

TEST(UnitSymbolCollector, OriginDirectiveChangesAddress)
{
	ASTDirective directive{ ".ORG", 5, };

	directive.args.emplace_back( ASTNumberExpr{0x300}, 0 );

    ASTProgram program;

	program.emplace_back( ASTElement{ 1, std::nullopt, std::move( directive ) } );
    program.emplace_back( ASTElement{ 2, ASTLabel{"start", 0}, ASTEmpty{} } );

    auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("start"), 0x300);
}

TEST(UnitSymbolCollector, EquDefinesConstant)
{
    ASTProgram program;

	program.emplace_back( ASTElement{ 1, std::nullopt, ASTEqu{ "SCREEN", 0, ASTExpression{ ASTNumberExpr{0xF00}, 0} } } );

    auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("SCREEN"), 0xF00);
}

TEST(UnitSymbolCollector, LabelIsAssignedBeforeElementAdvancesAddress)
{
    ASTProgram program;

    program.emplace_back( ASTElement{ 1, ASTLabel{"entry", 0}, ASTInstruction{"NOP", {}} } );

    auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("entry"), 0x200);
}

TEST(UnitSymbolCollector, TracksAddressesAcrossInstructionsAndData)
{
    ASTDirective data{ ".DB", 5 };

    data.args.emplace_back( ASTNumberExpr{0x11}, 0 );
    data.args.emplace_back( ASTNumberExpr{0x22}, 0 );

    ASTProgram program;

    program.emplace_back( ASTElement{ 1, std::nullopt, ASTInstruction{"NOP", {}} } );
    program.emplace_back( ASTElement{ 2, ASTLabel{"after_data", 0}, std::move(data) } );
    program.emplace_back( ASTElement{ 3, ASTLabel{"after_instruction", 0}, ASTInstruction{"NOP", {}} } );

    const auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("after_data"), 0x202);
    EXPECT_EQ(symbols.get_value("after_instruction"), 0x204);
}

TEST(UnitSymbolCollector, OriginDirectiveChangesSubsequentLabelAddress)
{
    ASTDirective origin { ".ORG", 5 };

	origin.args.emplace_back( ASTNumberExpr{0x300}, 0 );

	ASTProgram program;

    program.emplace_back( ASTElement{ 2, std::nullopt, std::move(origin) } );
    program.emplace_back( ASTElement{ 3, ASTLabel{"at_origin", 0}, ASTInstruction{"NOP", {}} } );

    const auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("at_origin"), 0x300);
}

TEST(UnitSymbolCollector, ConstantCanBeUsedByLaterOriginDirective)
{
    ASTDirective origin { ".ORG", 5 };

    origin.args.emplace_back( ASTIdentifierExpr{"start"}, 0 );

	ASTProgram program;

    program.emplace_back( ASTElement{ 2, std::nullopt, ASTEqu{"start", 5, ASTExpression{ASTNumberExpr{0x320}, 0} } } );
    program.emplace_back( ASTElement{ 3, std::nullopt, std::move(origin) } );
    program.emplace_back( ASTElement{ 4,  ASTLabel{"target", 0}, ASTInstruction{"NOP", {}} } );

    const auto symbols = collect_symbols(program);

    EXPECT_EQ(symbols.get_value("start"), 0x320);
    EXPECT_EQ(symbols.get_value("target"), 0x320);
}

TEST(UnitSymbolCollector, CannotForwardReferenceConstants)
{
    ASTDirective origin{ ".ORG", 5};

	origin.args.emplace_back( ASTNumberExpr{0x340}, 0 );

    ASTProgram program;

	program.emplace_back( ASTElement{ 1, std::nullopt, ASTEqu{"start", 0, ASTExpression{ASTIdentifierExpr{"target"}, 0}} });

	program.emplace_back( ASTElement{ 2, std::nullopt, std::move(origin) } );
	program.emplace_back( ASTElement{ 3, ASTLabel{"target", 0}, ASTInstruction{"NOP", {}} } );

	EXPECT_THROW( collect_symbols(program), std::runtime_error );
}

} // namespace
