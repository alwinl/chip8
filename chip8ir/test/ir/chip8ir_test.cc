/*
 * chip8ir_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/chip8ir.h"

class Chip8IRTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(Chip8IRTest, Construction)
{
    // Simple instructions
    auto instr_cls = Instruction::make_clear();
    auto instr_ret = Instruction::make_return();
    auto instr_jp  = Instruction::make_jump( Addr {0x200});
    auto instr_ld  = Instruction::make_ld( Reg {1}, Imm {0x42});
    auto instr_add = Instruction::make_add( Reg {1}, Imm {0x42});
    auto instr_drw = Instruction::make_drw( Reg {0}, Reg {1}, Nibble {5} );
    auto instr_skp = Instruction::make_skip_if_key( Key {2} );

    // Check operand counts
    EXPECT_EQ(instr_cls.operands().size(), 0);
    EXPECT_EQ(instr_ret.operands().size(), 0);
    EXPECT_EQ(instr_jp.operands().size(), 1);
    EXPECT_EQ(instr_ld.operands().size(), 2);
    EXPECT_EQ(instr_add.operands().size(), 2);
    EXPECT_EQ(instr_drw.operands().size(), 3);
    EXPECT_EQ(instr_skp.operands().size(), 1);

    // Equality tests
    auto instr_cls_dup = Instruction::make_clear();
    EXPECT_TRUE(instr_cls == instr_cls_dup);
    EXPECT_FALSE(instr_cls == instr_jp);

    auto instr_ld_dup = Instruction::make_ld( Reg {1}, Imm {0x42});
    auto instr_ld_diff = Instruction::make_ld( Reg {1}, Imm {0x43});
    EXPECT_TRUE(instr_ld == instr_ld_dup);
    EXPECT_FALSE(instr_ld == instr_ld_diff);

    // Stream output sanity
    std::stringstream ss;
    ss << instr_cls;
    EXPECT_NE(ss.str().find("CLS"), std::string::npos);

    ss.str("");
    ss << instr_ld;
    EXPECT_NE(ss.str().find("LD"), std::string::npos);
    EXPECT_NE(ss.str().find("V1"), std::string::npos);
    EXPECT_NE(ss.str().find("#66"), std::string::npos);

    ss.str("");
    ss << instr_drw;
    EXPECT_NE(ss.str().find("DRW"), std::string::npos);
    EXPECT_NE(ss.str().find("5"), std::string::npos);
}

TEST_F(Chip8IRTest, FullOpcodeCoverage)
{
    // Prepare a few sample values for operands
    Reg r0{0}, r1{1}, r2{2};
    Imm imm42{42};
    Addr addr200{0x200};
    Nibble nib5{5};
    Key key2{2};
    RegCount rc3{3};

    // List of all instructions to test
    std::vector<Instruction> instructions = {
        Instruction::make_clear(),
        Instruction::make_return(),
        Instruction::make_jump(addr200),
        Instruction::make_call(addr200),
        Instruction::make_skip_eq(r0, imm42),
        Instruction::make_skip_neq(r0, imm42),
        Instruction::make_skip_eq(r0, r1),
        Instruction::make_ld(r0, imm42),
        Instruction::make_add(r0, imm42),
        Instruction::make_ld(r0, r1),
        Instruction::make_or(r0, r1),
        Instruction::make_and(r0, r1),
        Instruction::make_xor(r0, r1),
        Instruction::make_add(r0, r1),
        Instruction::make_sub(r0, r1),
        Instruction::make_shift_right(r0, r1),
        Instruction::make_subn(r0, r1),
        Instruction::make_shift_left(r0, r1),
        Instruction::make_skip_neq(r0, r1),
        Instruction::make_ld_i(addr200),
        Instruction::make_jump_indexed(addr200),
        Instruction::make_rnd(r0, imm42),
        Instruction::make_drw(r0, r1, nib5),
        Instruction::make_skip_if_key(key2),
        Instruction::make_skip_not_key(key2),
        Instruction::make_load_delay_timer(r0),
        Instruction::make_store_key(r0),
        Instruction::make_store_delay_timer(r0),
        Instruction::make_load_sound_timer(r0),
        Instruction::make_add_i(r0),
        Instruction::make_sprite_for(r0),
        Instruction::make_bcd(r0),
        Instruction::make_save_regs(rc3),
        Instruction::make_load_regs(rc3)
    };

    for( auto& instr : instructions )
	{
        // Equality check: each instruction should equal itself
        EXPECT_TRUE(instr == instr);

        // Stream output sanity check
        std::stringstream ss;
        ss << instr;
        std::string output = ss.str();

        // Check that mnemonic is present
        EXPECT_NE( output.find("Opcode:"), std::string::npos );

        // Check that operands (if any) are present in output
        for( auto& op : instr.operands() ) {
            std::stringstream op_ss;
            op_ss << op;
            EXPECT_NE(output.find(op_ss.str()), std::string::npos);
        }
    }

 	// Cross-equality: no two different instructions should be equal
    for (size_t i = 0; i < instructions.size(); ++i) {
        for (size_t j = i + 1; j < instructions.size(); ++j) {
            EXPECT_FALSE(instructions[i] == instructions[j])
                << "Instructions at index " << i << " and " << j
                << " should not be equal!\n"
                << instructions[i] << "\n"
                << instructions[j];
        }
    }
}

TEST_F(Chip8IRTest, OperandEqualityFuzz)
{
    // Sample values for each operand type
    Reg r0{0}, r1{1};
    Addr a100{100}, a200{200};
    Imm i42{42}, i99{99};
    Nibble n5{5}, n10{10};
    Key k1{1}, k3{3};
    RegCount rc2{2}, rc5{5};

    std::vector<Operand> operands = {
        r0, r1,
        a100, a200,
        i42, i99,
        n5, n10,
        k1, k3,
        rc2, rc5
    };

    // Test all pairwise combinations
    for (size_t i = 0; i < operands.size(); ++i) {
        for (size_t j = 0; j < operands.size(); ++j) {
            if (i == j) {
                EXPECT_TRUE(operands[i] == operands[j])
                    << "Operand at index " << i << " should equal itself";
            } else {
                EXPECT_FALSE(operands[i] == operands[j])
                    << "Operands at index " << i << " and " << j << " should not be equal\n"
                    << operands[i] << " vs " << operands[j];
            }
        }
    }
}

TEST_F(Chip8IRTest, InstructionEqualityAndStream)
{
    // Sample operands
    Reg r0{0}, r1{1};
    Addr a100{100}, a200{200};
    Imm i42{42}, i99{99};
    Nibble n5{5}, n10{10};
    Key k1{1}, k3{3};
    RegCount rc2{2}, rc5{5};

    std::vector<Operand> operands = {r0, r1, a100, a200, i42, i99, n5, n10, k1, k3, rc2, rc5};

    // Build instructions with 0, 1, 2, 3 operands
    std::vector<Instruction> instructions;

    // 0-operand instructions (CLS, RET)
    instructions.push_back(Instruction::make_clear());
    instructions.push_back(Instruction::make_return());

    // 1-operand instructions
    for (auto& op : operands) {
        if (std::holds_alternative<Reg>(op)) {
            instructions.push_back(Instruction::make_load_delay_timer(std::get<Reg>(op)));
            instructions.push_back(Instruction::make_store_delay_timer(std::get<Reg>(op)));
            instructions.push_back(Instruction::make_add_i(std::get<Reg>(op)));
            instructions.push_back(Instruction::make_sprite_for(std::get<Reg>(op)));
            instructions.push_back(Instruction::make_bcd(std::get<Reg>(op)));
            instructions.push_back(Instruction::make_store_key(std::get<Reg>(op)));
        } else if (std::holds_alternative<Addr>(op)) {
            instructions.push_back(Instruction::make_jump(std::get<Addr>(op)));
            instructions.push_back(Instruction::make_call(std::get<Addr>(op)));
            instructions.push_back(Instruction::make_ld_i(std::get<Addr>(op)));
            instructions.push_back(Instruction::make_jump_indexed(std::get<Addr>(op)));
        } else if (std::holds_alternative<Key>(op)) {
            instructions.push_back(Instruction::make_skip_if_key(std::get<Key>(op)));
            instructions.push_back(Instruction::make_skip_not_key(std::get<Key>(op)));
        } else if (std::holds_alternative<RegCount>(op)) {
            instructions.push_back(Instruction::make_save_regs(std::get<RegCount>(op)));
            instructions.push_back(Instruction::make_load_regs(std::get<RegCount>(op)));
        }
    }

    // 2-operand instructions
    for (auto& op1 : operands) {
        for (auto& op2 : operands) {
            if (std::holds_alternative<Reg>(op1)) {
                if (std::holds_alternative<Reg>(op2)) {
                    instructions.push_back(Instruction::make_add(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_sub(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_shift_right(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_shift_left(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_xor(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_or(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_and(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_skip_eq(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_skip_neq(std::get<Reg>(op1), std::get<Reg>(op2)));
                    instructions.push_back(Instruction::make_ld(std::get<Reg>(op1), std::get<Reg>(op2)));
                } else if (std::holds_alternative<Imm>(op2)) {
                    instructions.push_back(Instruction::make_add(std::get<Reg>(op1), std::get<Imm>(op2)));
                    instructions.push_back(Instruction::make_skip_eq(std::get<Reg>(op1), std::get<Imm>(op2)));
                    instructions.push_back(Instruction::make_skip_neq(std::get<Reg>(op1), std::get<Imm>(op2)));
                    instructions.push_back(Instruction::make_ld(std::get<Reg>(op1), std::get<Imm>(op2)));
                    instructions.push_back(Instruction::make_rnd(std::get<Reg>(op1), std::get<Imm>(op2)));
                }
            }
        }
    }

    // 3-operand instructions
    for (auto& op1 : operands) {
        for (auto& op2 : operands) {
            for (auto& op3 : operands) {
                if (std::holds_alternative<Reg>(op1) &&
                    std::holds_alternative<Reg>(op2) &&
                    std::holds_alternative<Nibble>(op3)) {
                    instructions.push_back(Instruction::make_drw(
                        std::get<Reg>(op1), std::get<Reg>(op2), std::get<Nibble>(op3)));
                }
            }
        }
    }

	// Mnemonics array
    constexpr size_t opcode_count = (size_t)Opcode::LD_REGS + 1;
    static std::array<std::string, opcode_count> mnemonics = {
		"NOP", "CLS", "RET", "JP", "CALL", "SE",
		"SNE", "SE", "LD", "ADD", "LD", "OR", "AND",
		"XOR", "ADD", "SUB", "SHR", "SUBN", "SHL", "SNE", "LD I",
		"JP V0", "RND", "DRW", "SKP", "SKNP", "LD DT",
		"LD ST", "ST K", "ST DT", "ADD I",
		"LD F", "LD B", "ST [I]", "LD [I]"
    };

    // Test equality and streaming
    for (size_t i = 0; i < instructions.size(); ++i) {
        for (size_t j = 0; j < instructions.size(); ++j) {
            if (i == j) {
                EXPECT_TRUE(instructions[i] == instructions[j])
                    << "Instruction should equal itself: " << instructions[i];
            } else {
                EXPECT_FALSE(instructions[i] == instructions[j])
                    << "Instructions should not be equal:\n"
                    << instructions[i] << "\nvs\n" << instructions[j];
            }
        }

        // Check that the printed mnemonic matches the opcode
        std::ostringstream oss;
        oss << instructions[i];
        std::string out = oss.str();
		std::string mnemonic = mnemonics[static_cast<int>(instructions[i].opcode())];
        EXPECT_NE(out.find(mnemonic), std::string::npos) << "Mnemonic \"" << mnemonic << "\" not found in stream: " << out;
    }
}
